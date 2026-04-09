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

/** \class DelphesMultiThreadedReader
 *
 *  Reads Pythia8 event blocks
 *
 *  \author L. Forthomme - AGH, Krakow
 *
 */

#include "classes/DelphesReader.h"

class DelphesThreadWorker;

class DelphesMultiThreadedReader: public DelphesReader
{
public:
  explicit DelphesMultiThreadedReader(const std::vector<DelphesThreadWorker> &workerObjs, const DelphesReader &singleReaderObj);

  std::unique_ptr<DelphesReader> Clone() const override { return {}; }

  void LoadInputFile(std::string_view inputFile) override;
  void Clear() override;

private:
  std::vector<std::unique_ptr<DelphesReader> > fSingleReaders;
};
