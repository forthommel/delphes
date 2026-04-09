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
#include "classes/DelphesFormula.h"
#include "classes/DelphesMultiThreadedReader.h"
#include "classes/DelphesTCLConfReader.h"
#include "classes/DelphesThreadWorker.h"
#include "classes/DelphesWriter.h"

#include <ExRootAnalysis/ExRootProgressBar.h>

#include <TRandom3.h>

#include <chrono>

#ifdef _WIN32
#include <libloaderapi.h>
#else
#include <dlfcn.h>
#endif

Delphes::Delphes(const char *name) : DelphesModule(DelphesParameters{})
{
  SetName(name);
}

//------------------------------------------------------------------------------

void Delphes::Clear()
{
  for(DelphesThreadWorker &workerObj : fWorkers) workerObj.Clear();
  fReader->Clear();
}

//------------------------------------------------------------------------------

void Delphes::SetReader(DelphesReader *reader)
{
  fWorkers.clear(); // start by removing all modules registered
  const auto userConfig = fConfReader->Parameters();
  for(size_t i = 0; i < userConfig.Get<size_t>("NumThreads", 1); ++i)
    fWorkers.emplace_back(userConfig);
  fReader = std::make_unique<DelphesMultiThreadedReader>(fWorkers, *reader);
  fReader->Init();
}

//------------------------------------------------------------------------------

void Delphes::Reset()
{
  if(fReader) fReader->Reset();
}

//------------------------------------------------------------------------------

DelphesFactory *Delphes::GetFactory() const
{
  if(fWorkers.empty())
    throw std::runtime_error("Trying to retrieve factory while no worker is defined.");
  return fWorkers.at(0).GetFactory();
}

//------------------------------------------------------------------------------

void Delphes::Init()
{
  if(!fConfReader)
    throw std::runtime_error("Failed to initialise the main Delphes module with no user configuration reader declared.");

  const auto userConfig = fConfReader->Parameters();
  gRandom->SetSeed(userConfig.Get<int>("RandomSeed", 0));
}

//------------------------------------------------------------------------------

void Delphes::InitTask()
{
  for(DelphesThreadWorker &workerObj : fWorkers) workerObj.InitTask();
}

//------------------------------------------------------------------------------

void Delphes::ProcessTask()
{
  for(DelphesThreadWorker &workerObj : fWorkers) workerObj.ProcessTask();
}

//------------------------------------------------------------------------------

void Delphes::FinishTask()
{
  for(DelphesThreadWorker &workerObj : fWorkers) workerObj.FinishTask();
}

//------------------------------------------------------------------------------

void LoadLibrary(std::string_view libraryName)
{
#ifdef _WIN32
  if(HMODULE handle = LoadLibraryA(file_path.data()); handle == nullptr)
  {
    std::ostringstream message;
    message << "Failed to load library '" << file_path << "'. Error code #" << GetLastError() << ".";
    throw std::runtime_error(message.str());
  }
#else
  if(::dlopen(libraryName.data(), RTLD_LAZY | RTLD_GLOBAL) == nullptr)
  {
    std::ostringstream message;
    message << "Failed to load library '" << libraryName << "'.";
    if(const char *err = ::dlerror(); err != nullptr) message << " " << err;
    throw std::runtime_error(message.str());
  }
#endif
  std::cout << "Successfully loaded the library '" << libraryName << "' into the runtime environment." << std::endl;
}

//------------------------------------------------------------------------------
