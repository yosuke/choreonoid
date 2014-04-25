/**
   \file
   \author Shin'ichiro Nakaoka
*/

#include "PythonSimScriptItem.h"
#include <cnoid/PythonScriptItemImpl>
#include <cnoid/ItemManager>
#include <cnoid/Archive>
#include <boost/bind.hpp>
#include "gettext.h"

using namespace std;
using namespace boost;
using namespace cnoid;


void PythonSimScriptItem::initialize(ExtensionManager* ext)
{
    ext->itemManager().registerClass<PythonSimScriptItem>(N_("PythonSimScriptItem"));
    ext->itemManager().addLoader<PythonSimScriptItem>(
        _("Python Script for Simulation"), "PYTHON-SCRIPT-FILE", "py",
        bind(&PythonSimScriptItem::setScriptFilename, _1, _2));
}


PythonSimScriptItem::PythonSimScriptItem()
{
    impl = new PythonScriptItemImpl(this);
}


PythonSimScriptItem::PythonSimScriptItem(const PythonSimScriptItem& org)
    : SimulationScriptItem(org)
{
    impl = new PythonScriptItemImpl(this, *org.impl);
}


PythonSimScriptItem::~PythonSimScriptItem()
{
    delete impl;
}


void PythonSimScriptItem::onDisconnectedFromRoot()
{
    impl->onDisconnectedFromRoot();
}


bool PythonSimScriptItem::setScriptFilename(const std::string& filename)
{
    impl->setScriptFilename(filename);
    return true;
}


const std::string& PythonSimScriptItem::scriptFilename() const
{
    return impl->scriptFilename();
}


bool PythonSimScriptItem::setBackgroundMode(bool on)
{
    return impl->setBackgroundMode(on);
}


bool PythonSimScriptItem::isBackgroundMode() const
{
    return impl->isBackgroundMode();
}


bool PythonSimScriptItem::isRunning() const
{
    return impl->isRunning();
}


bool PythonSimScriptItem::executeAsSimulationScript()
{
    return impl->execute();
}


bool PythonSimScriptItem::executeCode(const char* code)
{
    return impl->executeCode(code);
}


bool PythonSimScriptItem::waitToFinish(double timeout)
{
    return impl->waitToFinish(timeout);
}
    

std::string PythonSimScriptItem::resultString() const
{
    return impl->resultString();
}


SignalProxy< boost::signal<void()> > PythonSimScriptItem::sigScriptFinished()
{
    return impl->sigScriptFinished();
}


bool PythonSimScriptItem::terminate()
{
    return impl->terminate();
}


ItemPtr PythonSimScriptItem::doDuplicate() const
{
    return new PythonSimScriptItem(*this);
}


void PythonSimScriptItem::doPutProperties(PutPropertyFunction& putProperty)
{
    impl->doPutProperties(putProperty);
    SimulationScriptItem::doPutProperties(putProperty);
}


bool PythonSimScriptItem::store(Archive& archive)
{
    if(SimulationScriptItem::store(archive) && impl->store(archive)){
        if(!lastAccessedFilePath().empty()){
            archive.writeRelocatablePath("file", lastAccessedFilePath());
        }
        return true;
    }
    return false;
}


bool PythonSimScriptItem::restore(const Archive& archive)
{
    if(SimulationScriptItem::restore(archive)){
        string filename;
        if(archive.readRelocatablePath("file", filename)){
            if(load(filename)){
                if(impl->restore(archive)){
                    return true;
                }
            }
        }
    }
    return false;
}
