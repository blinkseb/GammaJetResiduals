#pragma once

#include <cmath>
#include <vector>
#include <utility>

class PtBinning {
  public:
    PtBinning() {
      fillPtBins();
    }

    int getPtBin(float pt) {
      std::vector<std::pair<float, float> >::const_iterator it = mPtBins.begin();
      for (; it != mPtBins.end(); ++it) {
        std::pair<float, float> bin = *it;
        if (pt >= bin.first && pt < bin.second) {
          return it - mPtBins.begin();
        }
      }

      return -1;
    }

    size_t size() const {
      return mPtBins.size();
    }

    std::pair<float, float> getBinValue(int bin) const {
      return mPtBins[bin];
    }

    std::vector<std::pair<float, float> > getBinning(int n = -1) const {
      if (n < 0) {
        n = size();
      }
      return std::vector<std::pair<float, float> >(mPtBins.begin(), mPtBins.begin() + n);
    }

  private:
    std::vector<std::pair<float, float> > mPtBins;

    void fillPtBins() {
      mPtBins.push_back(std::make_pair(15., 22.));
      mPtBins.push_back(std::make_pair(22., 32.));
      mPtBins.push_back(std::make_pair(32., 53.));
      mPtBins.push_back(std::make_pair(53., 80.));
      mPtBins.push_back(std::make_pair(80., 100.));
      mPtBins.push_back(std::make_pair(100., 150.));
      mPtBins.push_back(std::make_pair(150., 220.));
      mPtBins.push_back(std::make_pair(220., 320.));
      mPtBins.push_back(std::make_pair(320., 470.));
      mPtBins.push_back(std::make_pair(470., 3500.));
      mPtBins.push_back(std::make_pair(3500., 10000.));
    }

};
