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

#ifndef DelphesThreadWorker_h
#define DelphesThreadWorker_h

/** \class DelphesThreadWorker
 *
 *  Main Delphes thread worker.
 *  Controls execution of all other modules.
 *
 *  \author L. Forthomme - AGH, Krakow
 *
 */

class DelphesModule;
class DelphesParameters;
class DelphesFactory;

class DelphesThreadWorker
{
public:
  DelphesThreadWorker(const DelphesParameters &);

  DelphesFactory *GetFactory() const;

  void Clear();
  void InitTask();
  void ProcessTask();
  void FinishTask();

protected:
  void ClearModules() { fModules.clear(); }
  void AddModule(std::string_view moduleName, std::unique_ptr<DelphesModule> &moduleObject);

private:
  const DelphesParameters fParams;
  std::unique_ptr<DelphesFactory> fFactory;

  std::vector<std::pair<std::string, std::unique_ptr<DelphesModule> > > fModules;
};

#endif
