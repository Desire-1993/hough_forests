/* 
// Author: Juergen Gall, BIWI, ETH Zurich
// Email: gall@vision.ee.ethz.ch
*/

#pragma once

#ifndef HAS_GALL
#define HAS_GALL
#endif

#include <cxcore.h>
#include <cv.h>

#include <vector>
#include <iostream>
#include <functional>

#include "HoG.h"

namespace gall {

// structure for image patch
struct PatchFeature {
	explicit PatchFeature() : src_index{-1} {}
	explicit PatchFeature(int frame) : src_index{frame} {}

    ~PatchFeature()
    {
        for (auto &patch : vPatch)
            cvReleaseMat(&patch);
    }

    bool const empty() const { return src_index == -1; }

    int            const src_index;  // index of the src image in the full training
	CvRect               roi;
	std::vector<CvPoint> center;
	std::vector<CvMat *> vPatch;

    // not copyable
    PatchFeature(PatchFeature const &)            = delete;

    // we need a move ctor to be storable in a vector<>
    PatchFeature(PatchFeature &&other)            = default;

    // not assignable because of the const index
    PatchFeature &operator=(PatchFeature &&)      = delete;
    PatchFeature &operator=(PatchFeature const &) = delete;

    // for template compatibility
    void create_channel_patches(int, int, std::vector<IplImage *> const &) {}
};

static HoG hog; 

class CRPatch {
public:
    CRPatch(CvRNG rng, int w, int h, int num_l) : rng(rng), width(w), height(h) { vLPatches.resize(num_l);}

    void add_patch(
        std::vector<IplImage *> const &vImg,
        int                            label,
        int                            src_index,
        cv::Point const               &pt,
        std::vector<CvPoint>    const *vCenter, int);

	// Extract patches from image
	void extractPatches(IplImage *img, unsigned int n, int label, CvRect const * const box = 0, std::vector<CvPoint>* vCenter = 0);

	// Extract patches from feature channels
	void extractPatches(std::vector<IplImage*> const &vImg,
                        unsigned int                  n,
                        int                           label,
                        CvRect const * const          box = 0,
                        std::vector<CvPoint>         *vCenter = 0);


	// Extract features from image
	static void extractFeatureChannels(IplImage *img, std::vector<IplImage*>& vImg);

	// min/max filter
	static void maxfilt(uchar* data, uchar* maxvalues, unsigned int step, unsigned int size, unsigned int width);
	static void maxfilt(uchar* data, unsigned int step, unsigned int size, unsigned int width);
	static void minfilt(uchar* data, uchar* minvalues, unsigned int step, unsigned int size, unsigned int width);
	static void minfilt(uchar* data, unsigned int step, unsigned int size, unsigned int width);
	static void maxminfilt(uchar* data, uchar* maxvalues, uchar* minvalues, unsigned int step, unsigned int size, unsigned int width);
	static void maxfilt(IplImage *src, unsigned int width);
	static void maxfilt(IplImage *src, IplImage *dst, unsigned int width);
	static void minfilt(IplImage *src, unsigned int width);
	static void minfilt(IplImage *src, IplImage *dst, unsigned int width);

    // for template compatibility
	CRPatch(CvRNG)                   { throw std::runtime_error("CRPatch::CRPatch(CvRNG) NOT IMPLEMENTED"); }
    void read(std::istream &)  const { throw std::runtime_error("CRPatch::read() NOT IMPLEMENTED"); }
    void write(std::ostream &) const { throw std::runtime_error("CRPatch::write() NOT IMPLEMENTED"); }


    using patch_adder_t = std::function<void (std::vector<IplImage *> const &,
                                              int,
                                              int,
                                              cv::Point const &,
                                              std::vector<CvPoint> const &)>;
	template<typename T>
	void extract_patches_of_texture(std::vector<IplImage *>         const &,
                                    T                               const &,
                                    std::vector<CvPoint>            const &,
                                    patch_adder_t                          = {},
                                    CvRect const                  * const  = nullptr)
    { throw std::runtime_error("CRPatch::extract_patches_of_texture() NOT IMPLEMENTED"); }

	int const width          = -1;
	int const height         = -1;
    int const LABEL_POSITIVE = 1;
    int const LABEL_NEGATIVE = 0;
	std::vector<std::vector<PatchFeature> > vLPatches;

private:
    CRPatch &operator=(CRPatch const &) = delete;

private:
	CvRNG rng;
};

}   // namespace gall
