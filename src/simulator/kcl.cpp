/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "kcl.h"

Kcl* Kcl::m_pSelf = nullptr;

Kcl::Kcl()
{
    m_pSelf = this;
    m_zero = 0.0;
    m_size = 0;
}
Kcl::~Kcl(){}

// Initialization time ------------------------------------------------------

void Kcl::createCells()
{
    m_nodeGroups.clear();

    m_callBacks.clear();
    m_callBacks.resize( m_size );

    m_fCallBacks.clear();
    m_fCallBacks.resize( m_size );

    m_voltage.clear();
    m_voltage.resize( m_size, 0.0 );

    m_coefVector.clear();
    std::vector<DataCell> currVector( m_size );
    m_coefVector = currVector;

    m_admiMatrix.clear();
    for( int i=0; i<m_size; ++i )  // Create cell matrix
    {
        cv_vector admitVector( m_size );
        admitVector.at(i).add = true;
        m_admiMatrix.emplace_back( admitVector );

        m_coefVector.at(i).add = true;
    }
}

void Kcl::initialize() // Split circuit in independent nets
{
    m_admiChanged.clear();
    m_currChanged.clear();
    m_aFaList.clear();
    m_aList.clear();
    m_bList.clear();
    m_vList.clear();

    int groups = m_nodeGroups.size();

    m_admiChanged.resize( groups, true );
    m_currChanged.resize( groups, true );

    m_aFaList.reserve( groups );
    m_aList.reserve( groups );
    m_bList.reserve( groups );
    m_vList.reserve( groups );

    for( int gNum=0; gNum<groups; ++gNum )
    {
        QList<int> nodes = m_nodeGroups.at( gNum );
        int gSize = nodes.size();
        cp_matrix admitMatrix;
        cp_vector currVector( gSize );
        dp_vector voltVector( gSize );

        for( int j=0; j<gSize; ++j ) // Populate group matrix with pointers to main matrix cells
        {
            int n0 = nodes.at(j);
            m_nodeMap[n0] = gNum;                 // Map node to group

            currVector.at(j) = &m_coefVector[n0]; // Point to main current vector
            voltVector.at(j) = &m_voltage[n0];    // Point to main voltage vector

            cp_vector admitVector( gSize );       // Create matrix row

            for( int k=0; k<gSize; ++k ){
                int n1 = nodes.at(k);
                admitVector[k] = &m_admiMatrix[n0][n1]; // Point row element to main matrix element
            }
            admitMatrix.emplace_back( admitVector );    // Addd row to matrix
        }
        m_aList.emplace_back( admitMatrix );            // Add group's admit matrix to list
        m_bList.emplace_back( currVector );             // Add group's current vector to list
        m_vList.emplace_back( voltVector );             // Add group's voltage vector to list

        dv_matrix aFaMatrix( gSize, dv_vector( gSize, 0.0 ) ); // Factored matrix for this group
        m_aFaList.emplace_back( aFaMatrix );                   // Add group factored matrix to list
    }

}

void Kcl::preCalculate()
{
    /// All candidate groups mut be precalculated
    /// Iterate through all sources to find all possible combinations:
    /// - Call each source to set a predefined state at each step
    /// - Solve the system normally and save the voltage vector
    /// Create a solution (voltage vector) for each input combination
    /// Input combinations are saved as input vectors of state indexes.
    /// Map input->solution.
    ///
    /// At run time Sources should set its state index instead of stamp.
    /// Solution is retrieved from Map and voltages set normally.
}

void Kcl::addAdmitance( double* a, int x, int y ) // Add pointers to admit
{
    m_admiMatrix[x][x].values.emplace_back(a);
    if( y >= 0 ){
        m_admiMatrix[x][y].values.emplace_back(a);
        m_admiMatrix[y][y].values.emplace_back(a);
        m_admiMatrix[y][x].values.emplace_back(a);
    }

    admitChanged( x, y ); // First time calculate everything

    // Create node groups:
    // will be used to divide circuit in smaller unconnected parts.

    int groupX = -1;
    int groupY = -1;

    for( int i=0; i<m_nodeGroups.size(); ++i )  // Fing groups containing nodes x, y
    {
        QList<int> nodeGroup = m_nodeGroups.at(i);
        if( nodeGroup.contains(x) ) groupX = i;
        if( nodeGroup.contains(y) ) groupY = i;
    }
    bool cx = groupX != -1;
    bool cy = groupY != -1;

    if( cx && cy )                            // 2 groups
    {
        if( groupX != groupY ){               // 2 different groups: Joing groups
            if( groupY < groupX ){            // Join to the smallest index
                std::swap( groupX, groupY );
            }
            m_nodeGroups[groupX].append( m_nodeGroups[groupY] ); // All to group X
            m_nodeGroups.removeAt( groupY );                     // Remove group Y
        }
        return;
    }
    if( cx || cy )                            // One node found in a group
    {
        if( !cx )           m_nodeGroups[groupY].append(x);
        if( !cy && y >= 0 ) m_nodeGroups[groupX].append(y);
        return;
    }

    QList<int> nodeGroup;                     // Nodes not found, create a new group
    nodeGroup.append(x);
    if( y >= 0 ) nodeGroup.append(y);
    m_nodeGroups.append( nodeGroup );
}

bool* Kcl::addCurrent( double* c, int n )
{
    m_coefVector[n].values.emplace_back(c);
    m_coefVector[n].changed = true;
    int group = m_nodeMap[n];
    m_currChanged[group] = true;
    //currentChanged( m_nodeMap[n] );
    return &m_coefVector[n].changed;
}

// Run time -------------------------------------------------------------

void Kcl::addChangeCB( Element* cb, int n ) // Add a change callback to node n
{
    std::vector<Element*>* list = &m_callBacks[n];
    for( auto it = list->begin(); it!=list->end(); ++it )
    {
        if( *it == cb ) return; // CallBack already in the list
    }
    m_callBacks[n].emplace_back(cb);
}

void Kcl::addFblockChangeCB( Element* cb, int n ) // Add a change callback to node n
{
    std::vector<Element*>* list = &m_fCallBacks[n];
    for( auto it = list->begin(); it!=list->end(); ++it )
    {
        if( *it == cb ) return; // CallBack already in the list
    }
    m_fCallBacks[n].emplace_back(cb);
}

void Kcl::remChangeCB( Element* cb, int n ) // Remove a change callback from node n
{
    std::vector<Element*>* list = &m_callBacks[n];
    for( auto it = list->begin(); it!=list->end(); ++it )
    {
        if( *it == cb ) { list->erase(it); break; }
    }
}

void Kcl::admitChanged( int x, int y, int group )
{
    m_admiMatrix[x][x].changed = true;
    if( y >= 0 ){
        m_admiMatrix[x][y].changed = true;
        m_admiMatrix[y][y].changed = true;
        m_admiMatrix[y][x].changed = true;
    }
    if( group >= 0 ) m_admiChanged[group] = true; /// Check
}

bool Kcl::solveSystem()
{
    bool ok = true;
    for( uint i=0; i<m_admiChanged.size(); ++i ) // Iterate through groups
    {
        if( !m_currChanged[i] && !m_admiChanged[i] ) continue;

        if( m_admiChanged[i] ) factorMatrix(i);
        if( !luSolve(i) ) ok = false;

        m_currChanged[i] = false;
        m_admiChanged[i] = false;
    }
    return ok;
}

void Kcl::factorMatrix( int group ) // Factor matrix into Lower/Upper triangular
{
    cp_matrix& ap = m_aList[group];
    dv_matrix& av = m_aFaList[group];

    int n = av.size();

    int row,col,k;

    for( col=0; col<n; ++col )                  // Crout's method: loop through columns
    {
        for( row=0; row<col; ++row )            // Upper triangular elements
        {
            DataCell* ac = ap[row][col];
            if( ac->changed ) ac->updateVal();  // Recalculate total admitance

            double q = ac->total;
            for( k=0; k<row; ++k ) q -= av[row][k]*av[k][col];
            av[row][col] = q;
        }
        for( row=col; row<n; ++row )            // Lower triangular elements
        {
            DataCell* ac = ap[row][col];
            if( ac->changed ) ac->updateVal();  // Recalculate total admitance

            double q = ac->total;
            for( k=0; k<col; ++k ) q -= av[row][k]*av[k][col];
            av[row][col] = q;
        }
        if( col != n-1 )                        // Normalize column respect to diagonal
        {
            double div = av[col][col];
            if( div == 0 ) continue;
            for( row=col+1; row<n; ++row ) av[row][col] /= div;
        }
    }
}

bool Kcl::luSolve( int group ) // Solve the system to get voltages for each node
{
    dv_matrix& av = m_aFaList[group];
    cp_vector& bp = m_bList[group];
    dp_vector& vp = m_vList[group];

    int n = av.size();

    dv_vector b;
    b.resize( n, 0 );

    double tot;
    int i;
    for( i=0; i<n; ++i )
    {
        DataCell* bc = bp[i];
        if( bc->changed ) bc->updateVal();   // Recalculate total current for this node
        tot = bc->total;
        if( tot != 0 ){ b[i] = tot; break; } // First nonzero b element
    }

    int bi = i++;
    for( ; i<n; ++i )
    {
        DataCell* bc = bp[i];
        if( bc->changed ) bc->updateVal();   // Recalculate total current for this node
        tot = bc->total;

        dv_vector& avRow = av.at(i);
        for( int j=bi; j<i; ++j ) tot -= avRow[j]*b[j]; // Forward substitution from lower triangular matrix
        b[i] = tot;
    }
    bool isOk = true;

    QList<int>* nodeGroup = &m_nodeGroups[group];
    Element* firstCB  = nullptr;              // Callback list
    Element* firstFcB = nullptr;              // fCallback list
    for( i=n-1; i>=0; --i )
    {
        dv_vector& avRow = av.at(i);
        tot = b[i];
        for( int j=i+1; j<n; ++j ) tot -= avRow[j]*b[j]; // Back substitution from upper triangular matrix

        double div = avRow[i];
        double volt = 0;
        if( div != 0 ) volt = tot/div;
        else volt = 0; //isOk = false;

        b[i] = volt;

        if( volt == *vp[i] ) continue;   // Voltage at this node didn't change
        *vp[i] = volt;

        int node = nodeGroup->at(i);
        std::vector<Element*>* list = &m_callBacks[node];
        for( Element* e : *list )                     // Find callbacks for this node and add it to list (avoids calling them several times)
        {
            Element* f = firstCB;
            while( f )               // Add element to the list if not already there
            {
                if( e == f ) break;  // Calback Already in the list
                f = f->next;
            }
            if( f ) continue;        // Callback was in the list

            e->next = firstCB;       // Add callback to the list
            firstCB = e;
        }
        std::vector<Element*>* fList = &m_fCallBacks[node];
        for( Element* e : *fList )   // Same for fComponents
        {
            Element* f = firstFcB;
            while( f )               // Add element to the list if not already there
            {
                if( e == f ) break;  // Calback Already in the list
                f = f->next;
            }
            if( f ) continue;        // Callback was in the list

            e->next = firstFcB;       // Add callback to the list
            firstFcB = e;
        }
    }

    while( firstCB )                 // Call voltage changed callbacks for all nodes with changes
    {
        firstCB->voltChanged();
        firstCB = firstCB->next;
    }
    while( firstFcB )                 // Call fComponents for internal update
    {
        firstFcB->voltChanged();
        firstFcB = firstFcB->next;
    }
    return isOk;
}
