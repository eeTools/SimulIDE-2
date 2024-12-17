/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef KCL_H
#define KCL_H

#include <vector>
#include <map>
#include <QList>

#include "element.h"

class Kcl
{
    class DataCell  // Cell in Admitance Matrix: holds pointers to individual admitances and total admitance
    {               // Total admitance is the actual value in Admitance matrix used to solve circuit
      public:
        DataCell(){
            add     = false;
            changed = false;
            total   = 0;
        }

        std::vector<double*> values;  // Admitance vector
        double total;                 // Total admitance

        void updateVal(){             // Recalculate total admitance value for this matrix cell
            total = 0;
            if( add ) for( double* d : values ) total += *d; // Diagonals use positive values
            else      for( double* d : values ) total -= *d; // Non diagonal use negative values
            changed = false;
        }

        bool add;
        bool changed;
    };

    typedef std::vector<double>    dv_vector;
    typedef std::vector<double*>   dp_vector;
    typedef std::vector<dv_vector> dv_matrix;
    typedef std::vector<dp_vector> dp_matrix;

    typedef std::vector<DataCell>  cv_vector;
    typedef std::vector<DataCell*> cp_vector;
    typedef std::vector<cv_vector> cv_matrix;
    typedef std::vector<cp_vector> cp_matrix;

    public:
        Kcl();
        ~Kcl();

 static Kcl* self() { return m_pSelf; }

        void addAdmitance( double* a, int x, int y=-1 ); // Add admitance betwen two nodes
        bool* addCurrent( double* c, int n );          // Add current to node

        void addChangeCB( Element* c, int n );  // Add change callback: when Node voltage changes, Element function is called
        void remChangeCB( Element* c, int n );  // Remove change callback

        void addFblockChangeCB( Element* cb, int n ); // Add callback for fComponents

        void admitChanged( int x, int y=-1, int group=-1 );
        //void currentChanged( int node );

        int getNumGroups() { return m_nodeGroups.size(); }
        int getGroup( int node ) { return m_nodeMap[node]; }
        double getVoltage( int n ) { return m_voltage[n]; }

        int* getCurrentGroupChanged( int group ) { return &m_currChanged[group];}

        void setSize( int n ) { m_size = n; }

    protected:
        void createCells();
        void initialize();
        void preCalculate();
        void solveSystem();

    private:
 static Kcl* m_pSelf;

        inline void factorMatrix( int group );
        inline void luSolve( int group );
        inline Element* findCalbacks( std::vector<Element*> *list );

        int m_size;

        cv_matrix m_admiMatrix;           // Main admitance matrix (cells)
        std::vector<cp_matrix> m_aList;   // List of group admitance matrix (cells)
        std::vector<dv_matrix> m_aFaList; // List of group factored matrix (double)

        cv_vector m_coefVector;           // Main current vector (cells)
        std::vector<cp_vector> m_bList;   // List of group currents (cells)

        dv_vector m_voltage;              // Node voltage vector
        std::vector<dp_vector> m_vList;   // List of group node voltage

        QList<QList<int>> m_nodeGroups;   // List of node groups (unconnected sections)
        std::map<int,int> m_nodeMap;      // Map nodes to groups

        std::vector<bool> m_admiChanged;  // Some group's admit changed
        std::vector<int>  m_currChanged;  // Some group's current changed

        std::vector<std::vector<Element*>> m_callBacks;  // Callback list for each node
        std::vector<std::vector<Element*>> m_fCallBacks;  // fComponent Callback list for each node

        double m_zero;
};

#endif
