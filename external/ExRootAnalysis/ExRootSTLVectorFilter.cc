
/** \class ExRootSTLVectorFilter
 *
 *  Class simplifying classification and subarrays handling
 *
 *  \author P. Demin - UCL, Louvain-la-Neuve
 *          L. Forthomme - AGH, Krakow
 *
 */

#include "ExRootAnalysis/ExRootClassifier.h"
#include "ExRootAnalysis/ExRootSTLVectorFilter.h"

#include "classes/DelphesClasses.h" //TODO: should not depend on Delphes objects... make this templated?

#include <stdexcept>

ExRootSTLVectorFilter::ExRootSTLVectorFilter(const std::vector<Candidate *> &collection) :
  fCollection(collection)
{
}

//------------------------------------------------------------------------------

void ExRootSTLVectorFilter::Reset(ExRootClassifier *classifier)
{
  if(classifier)
  {
    if(auto itMap = fMap.find(classifier); itMap != fMap.end())
    {
      itMap->second.first = true;
      for(auto itSubMap = itMap->second.second.begin(); itSubMap != itMap->second.second.end(); ++itSubMap)
        itSubMap->second.clear();
    }
  }
  else
  {
    for(auto itMap = fMap.begin(); itMap != fMap.end(); ++itMap)
    {
      itMap->second.first = true;
      for(auto itSubMap = itMap->second.second.begin(); itSubMap != itMap->second.second.end(); ++itSubMap)
        itSubMap->second.clear();
    }
  }
}

//------------------------------------------------------------------------------

std::vector<Candidate *> ExRootSTLVectorFilter::GetSubArray(ExRootClassifier *classifier, Int_t category)
{
  auto itMap = fMap.find(classifier);
  if(itMap == fMap.end()) // classifier was not found
  {
    auto pairMap = fMap.insert(std::make_pair(classifier, std::make_pair(true, TCategoryMap())));
    if(!pairMap.second)
      throw std::runtime_error("can't insert category map");
    itMap = pairMap.first;
  }

  if(itMap->second.first)
  {
    itMap->second.first = false;
    for(const auto &element : fCollection)
    {
      const auto result = classifier->GetCategory(const_cast<TObject *>(static_cast<const TObject *>(element)));
      if(result < 0) continue;
      auto itSubMap = itMap->second.second.find(result);
      if(itSubMap == itMap->second.second.end())
      {
        auto pairSubMap = itMap->second.second.insert(std::make_pair(result, std::vector<Candidate *>(fCollection.size())));
        if(!pairSubMap.second)
        {
          throw std::runtime_error("can't insert category");
        }

        itSubMap = pairSubMap.first;
      }
      itSubMap->second.emplace_back(element);
    }
  }
  if(auto itSubMap = itMap->second.second.find(category); itSubMap != itMap->second.second.end())
    return itSubMap->second;
  return std::vector<Candidate *>{};
}

//------------------------------------------------------------------------------
