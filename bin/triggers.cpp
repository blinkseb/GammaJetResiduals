#include <iostream>
#include <string>
#include <exception>
#include <cassert>

#include "tinyxml2.h"

#include "triggers.h"

using namespace tinyxml2;

bool Triggers::parse() {
  XMLDocument doc;
  if (doc.LoadFile(mXmlFile.c_str())) {
    doc.PrintError();
    return false;
  }

  const XMLElement* root = doc.FirstChildElement("triggers");
  if (! root)
    return false;

  const XMLElement* runs = root->FirstChildElement("runs");
  for (; runs; runs = runs->NextSiblingElement("runs")) {
    parseRunsElement(runs);
  }

  return true;
}

bool Triggers::parseRunsElement(const XMLElement* runs) {
  Range<unsigned> runRange(runs->UnsignedAttribute("from"), runs->UnsignedAttribute("to"));

  PathVector runPaths;

  const XMLElement* paths = runs->FirstChildElement("path");
  for (; paths; paths = paths->NextSiblingElement("path")) {
    const std::string name = paths->FirstChildElement("name")->GetText();
    const XMLElement* pt = paths->FirstChildElement("pt");

    Range<float> ptRange(pt->FloatAttribute("from"), pt->FloatAttribute("to"));

    const XMLElement* weightElement = paths->FirstChildElement("weight");
    float weight = weightElement->FloatAttribute("value");

    Trigger t { ptRange, weight };

    runPaths.push_back(std::make_pair(boost::regex(name, boost::regex_constants::icase), t));
  }

  mTriggers[runRange] = runPaths;
  return true;
}

const PathVector& Triggers::getTriggers(unsigned int run) {
  if (mCachedRange && mCachedRange->in(run)) {
    return *mCachedVector;
  }

  for (auto& trigger: mTriggers) {
    const Range<unsigned int>& runRange = trigger.first;

    if (runRange.in(run)) {

      mCachedRange = &runRange;
      mCachedVector = &trigger.second;

      return *mCachedVector;
    }
  }

  std::cout << "Error: run " << run << " not found for triggers selection" << std::endl;
  assert(false);
}

/*const Regexp& Triggers::getHLTPath(unsigned int run, float pt) {
  const PathVector* paths = NULL;
  if (mCachedRange && mCachedRange->in(run)) {
    paths = mCachedVector;
  } else {
    for (auto& trigger: mTriggers) {
      const Range<unsigned int>& runRange = trigger.first;

      if (runRange.in(run)) {
        mCachedRange = &runRange;
        mCachedVector = &trigger.second;
        paths = mCachedVector;
        break;
      }
    }
  }

  if (! paths) {
    throw new std::exception();
  }

  for (auto& path: *paths) {
    const Range<float>& ptRange = path.second;
    if (ptRange.in(pt)) {
      return path.first;
    }
  }

  throw new std::exception(); // Not found
}*/


void Triggers::print() {
  for (auto& trigger: mTriggers) {
    const Range<unsigned int>& runRange = trigger.first;
    const auto& paths = trigger.second;

    std::cout << "Runs: " << runRange << std::endl;
    for (auto& path: paths) {
      std::cout << path.first << " -> " << path.second.range << "; weight: " << path.second.weight << std::endl;
    }
  }
}

//--------


bool MCTriggers::parse() {
  XMLDocument doc;
  if (doc.LoadFile(mXmlFile.c_str())) {
    doc.PrintError();
    return false;
  }

  const XMLElement* root = doc.FirstChildElement("triggers");
  if (! root)
    return false;

  const XMLElement* path = root->FirstChildElement("path");
  for (; path; path = path->NextSiblingElement("path")) {
    parsePathElement(path);
  }

  return true;
}

bool MCTriggers::parsePathElement(const XMLElement* path) {

  // Parse pt
  const XMLElement* pt = path->FirstChildElement("pt");
  Range<float> ptRange(pt->FloatAttribute("from"), pt->FloatAttribute("to"));

  // Parse names
  const XMLElement* name = path->FirstChildElement("name");
  for (; name; name = name->NextSiblingElement("name")) {
    const std::string n = name->GetText();
    double weight = 1.;
    name->QueryDoubleAttribute("weight", &weight);

    MCTrigger t {boost::regex(n, boost::regex_constants::icase), weight};
    mTriggers[ptRange].push_back(t);
  }

  return true;
}

void MCTriggers::print() {
  for (auto& trigger: mTriggers) {
    const Range<float>& ptRange = trigger.first;
    const auto& paths = trigger.second;

    std::cout << "Pt range: " << ptRange << std::endl;
    for (auto& path: paths) {
      std::cout << path.name << " -> " << path.weight << std::endl;
    }
  }
}
