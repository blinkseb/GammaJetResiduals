#pragma once

#include <iostream>

#include <map>
#include <utility>
#include <vector>

#include "tinyxml2.h"

#include <boost/regex.hpp>

template<typename T>
class Range {
  public:
    Range(T from, T to):
      mFrom(from), mTo(to) {}

    T from() const {
      return mFrom;
    }

    T to() const {
      return mTo;
    }

    bool in(T value) const {
      /*if (mFrom < 0)
        return value <= mTo;
      else if (mTo < 0)
        return value >= mFrom;
      else*/
        return value >= mFrom && value <= mTo;
    }

    bool operator<(const Range<T>& other) const {
      return mFrom < other.mFrom;
    }

    template<typename U>
    friend std::ostream& operator<<(std::ostream& stream, const Range<U>& range);

  private:
    T mFrom;
    T mTo;
};

struct Trigger {
  Range<float> range;
  float weight;
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Range<T>& range)
{
  stream << "[" << range.from() << ", " << range.to() << "]";

  return stream;
}

typedef std::pair<boost::regex, Trigger> PathData;
typedef std::vector<PathData> PathVector;

class Triggers {
  public:
    Triggers(const std::string& xmlFile):
      mXmlFile(xmlFile), mCachedRange(NULL), mCachedVector(NULL) {}

    bool parse();
    void print();

    //const boost::regex& getHLTPath(unsigned int run, float pt);
    const PathVector& getTriggers(unsigned int run);

  private:
    std::string mXmlFile;
    std::map<Range<unsigned int>, PathVector> mTriggers;

    const Range<unsigned int>* mCachedRange;
    const PathVector* mCachedVector;

    bool parseRunsElement(const tinyxml2::XMLElement* runs);
};

struct MCTrigger {
  boost::regex name;
  double weight;
};

class MCTriggers {
  public:
    MCTriggers(const std::string& xmlFile):
      mXmlFile(xmlFile) {}

    bool parse();
    void print();

    //const boost::regex& getHLTPath(unsigned int run, float pt);
    const std::map<Range<float>, std::vector<MCTrigger>>& getTriggers() {
      return mTriggers;
    }

  private:
    std::string mXmlFile;
    std::map<Range<float>, std::vector<MCTrigger>> mTriggers;

    bool parsePathElement(const tinyxml2::XMLElement* path);
};
