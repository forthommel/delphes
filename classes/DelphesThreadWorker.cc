/*
 *  Delphes: a framework for fast simulation of a generic collider experiment
 *  Copyright (C) 2012-2014  Universite catholique de Louvain (UCL), Belgium
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \class Delphes
 *
 *  Main Delphes module.
 *  Controls execution of all other modules.
 *
 *  \author P. Demin - UCL, Louvain-la-Neuve
 *
 */

#include "modules/Delphes.h"

#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "classes/DelphesThreadWorker.h"

#include <ExRootAnalysis/ExRootProgressBar.h>

#include <TRandom3.h>

#include <chrono>

DelphesThreadWorker::DelphesThreadWorker(const DelphesParameters &params) :
  fParams(params), fFactory(std::make_unique<DelphesFactory>()) {}

//------------------------------------------------------------------------------

void DelphesThreadWorker::AddModule(std::string_view moduleName, std::unique_ptr<DelphesModule> &moduleObject)
{
  fModules.emplace_back(std::make_pair(moduleName, std::move(moduleObject)));
}

//------------------------------------------------------------------------------

DelphesFactory *DelphesThreadWorker::GetFactory() const
{
  if(!fFactory) throw std::runtime_error("No factory was defined for this worker.");
  return fFactory.get();
}

//------------------------------------------------------------------------------

void DelphesThreadWorker::Clear() { fFactory->Clear(); }

//------------------------------------------------------------------------------

void DelphesThreadWorker::InitTask()
{
  /*if(!GetReader())
    throw std::runtime_error("Failed to initialise the main Delphes module with no reader declared.");*/

  ClearModules(); // start by removing all modules registered

  gRandom->SetSeed(fParams.Get<int>("RandomSeed", 0));

  for(const std::string &moduleName : fParams.Get<std::vector<std::string> >("ExecutionPath"))
  {
    if(!fParams.Has<DelphesParameters>(moduleName))
    {
      std::ostringstream message;
      message << "module '" << moduleName;
      message << "' is specified in ExecutionPath but not configured.";
      throw std::runtime_error(message.str());
    }
    try
    {
      const DelphesParameters moduleParams = fParams.Get<DelphesParameters>(moduleName);
      const std::string moduleTypeFromParams = moduleParams.Get<std::string>("ModuleType", moduleName);
      std::unique_ptr<DelphesModule> moduleObject = DelphesProcessingModuleFactory::Get().Build(moduleTypeFromParams, moduleParams);
      if(moduleObject->IsWriter()) continue; // we leave the writer modules for master Delphes module
      moduleObject->SetName(moduleName);
      moduleObject->SetFactory(fFactory.get());
      std::cout << std::left;
      std::cout << std::setw(30) << "** INFO: initializing module";
      std::cout << std::setw(25) << moduleName << std::endl;
      moduleObject->Init();
      AddModule(moduleName, moduleObject);
    }
    catch(const std::runtime_error &error)
    {
      std::ostringstream message;
      message << "Failed to build '" << moduleName << "' module. Error: " << error.what();
      if(fParams.Has<DelphesParameters>(moduleName))
        message << "\nParameters:\n"
                << fParams.Get<DelphesParameters>(moduleName);
      throw std::runtime_error(message.str());
    }
  }
}

//------------------------------------------------------------------------------

void DelphesThreadWorker::ProcessTask()
{
  std::chrono::time_point<std::chrono::high_resolution_clock> procTimer =
    std::chrono::high_resolution_clock::now();
  for(const auto &[moduleName, moduleObject] : fModules)
    moduleObject->Process();
  fFactory->SetProcessingTime(std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - procTimer)
      .count());
}

//------------------------------------------------------------------------------

void DelphesThreadWorker::FinishTask()
{
  for(const auto &[moduleName, moduleObject] : fModules)
    moduleObject->Finish();
}

//------------------------------------------------------------------------------
