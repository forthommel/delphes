/*
 *  Delphes: a framework for fast simulation of a generic collider experiment
 *  Copyright (C) 2012-2014  Universite catholique de Louvain (UCL), Belgium
 *                2026       AGH University of Krakow, Poland
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

#ifndef DelphesModel_h
#define DelphesModel_h

#include <ROOT/RField.hxx>

#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>

/** \class DelphesModel
 *
 *  Collection of fields model
 *
 *  \author L. Forthomme - AGH, Krakow
 *  \note Shamelessly (self-)stolen from the TDAnalyser memory management layout
 *
 */

using namespace ROOT; // from ROOT v6.36 on, default namespace
using namespace ROOT::Experimental;

/// Handle to retrieve fields from a data model
template <typename T>
using InputHandle = std::shared_ptr<T>;
/// Handle to store fields into a data model
template <typename T>
using OutputHandle = std::shared_ptr<T>;

class DelphesModel
{
public:
  explicit DelphesModel(std::string_view model_name);
  ~DelphesModel();

  class FieldName
  {
  public:
    explicit FieldName(std::string_view label);

    const std::string &FieldLabel() const { return fFieldLabel; }

  private:
    std::string fLabel;
    std::string fFieldLabel;
  };

  /// Book a memory segment, and attach it to a given field identified by a field name and a description
  template <typename T>
  void Book(OutputHandle<T> &handle, std::string_view field_name, std::string_view description = "")
  {
    const auto field_name_str = std::string{field_name};
    if(!fFields.insert(field_name_str).second)
      throwBookingFailure(field_name, "Field name already exists");
    const auto field_label = FieldName(field_name).FieldLabel();
    if(fFieldTypes.count(field_label) == 0)
      fFieldTypes[field_label] = RField<T>::TypeName();
    handle = std::make_shared<T>();
    fMemorySegments[field_label] = reinterpret_cast<void *>(handle.get());
  }

  /// Attach a memory segment to a given field identified by a field name
  template <typename T>
  void Attach(std::string_view field_name, InputHandle<T> &handle) const
  {
    if(const auto field_name_str = FieldName(field_name).FieldLabel(); fMemorySegments.count(field_name_str) == 0)
      throwAttachingFailure(field_name, "Collection name is not available");
    else
      handle.reset(reinterpret_cast<T *>(fMemorySegments.at(field_name_str)));
  }

private:
  void throwBookingFailure(std::string_view, std::string_view details = "") const noexcept(false);
  void throwAttachingFailure(std::string_view, std::string_view details = "") const noexcept(false);

  const std::string fName; ///< fields collection name (run, event/trigger)
  std::set<std::string> fFields; ///< collection of field names managed by this data model
  /// mapping of C++ types associated to each field registered in the data model
  std::unordered_map<std::string, std::string> fFieldTypes;
  /// mapping of memory segments associated to each field registered in the data model
  std::unordered_map<std::string, void *> fMemorySegments;
};

#endif /* DelphesModel_h */
