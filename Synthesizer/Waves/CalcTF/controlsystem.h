#ifndef CONTROLSYSTEM_H
#define CONTROLSYSTEM_H

#include "transferfunction.h"
#include <memory>
#include "roottablemodel.h"
#include "intervaltf.h"



class ControlSystem
{
public:
    ControlSystem();

    RootTableModel *getRootTModel() const;

    void updateRootTable();


    std::shared_ptr<IntervalTF> getCsTF() ;
    void setCsTF(std::shared_ptr<IntervalTF> csTF);

private:

    std::shared_ptr<IntervalTF> m_csTF; //by system I mean a feed back control system with plant, feedback and control transfer functions

    RootTableModel *m_rootTModel;

};

#endif // CONTROLSYSTEM_H
