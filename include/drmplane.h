/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_DRM_PLANE_H_
#define ANDROID_DRM_PLANE_H_

#include "drmcrtc.h"
#include "drmproperty.h"

#include <stdint.h>
#include <xf86drmMode.h>
#include <vector>

namespace android {

enum DrmPlaneType{
      DRM_PLANE_TYPE_CLUSTER0_WIN0 = 1 << 0,
      DRM_PLANE_TYPE_CLUSTER0_WIN1 = 1 << 1,
      DRM_PLANE_TYPE_CLUSTER1_WIN0 = 1 << 2,
      DRM_PLANE_TYPE_CLUSTER1_WIN2 = 1 << 3,
      DRM_PLANE_TYPE_ESMART0_WIN0 = 1 << 4,
      DRM_PLANE_TYPE_ESMART1_WIN0 = 1 << 5,
      DRM_PLANE_TYPE_SMART0_WIN0 = 1 << 6,
      DRM_PLANE_TYPE_SMART1_WIN0 = 1 << 7,
      DRM_PLANE_TYPE_Unknown = 0xff,
};

struct plane_type_name {
  DrmPlaneType type;
  const char *name;
};

enum DrmPlaneRotationType{
      DRM_PLANE_ROTATION_0 = 1 << 0,
      DRM_PLANE_ROTATION_90 = 1 << 1,
      DRM_PLANE_ROTATION_270 = 1 << 2,
      DRM_PLANE_ROTATION_X_MIRROR = 1 << 3,
      DRM_PLANE_ROTATION_Y_MIRROR = 1 << 4,
      DRM_PLANE_ROTATION_Unknown = 0xff,
};

struct plane_rotation_type_name {
  DrmPlaneRotationType type;
  const char *name;
};


enum DrmPlaneFeatureType{
      DRM_PLANE_FEARURE_SCALE   = 0,
      DRM_PLANE_FEARURE_ALPHA   = 1,
      DRM_PLANE_FEARURE_HDR2SDR = 2,
      DRM_PLANE_FEARURE_SDR2HDR = 3,
      DRM_PLANE_FEARURE_AFBDC   = 4,
};

enum DrmPlaneFeatureTypeBit{
      DRM_PLANE_FEARURE_BIT_SCALE   = 1 << DRM_PLANE_FEARURE_SCALE,
      DRM_PLANE_FEARURE_BIT_ALPHA   = 1 << DRM_PLANE_FEARURE_ALPHA,
      DRM_PLANE_FEARURE_BIT_HDR2SDR = 1 << DRM_PLANE_FEARURE_HDR2SDR,
      DRM_PLANE_FEARURE_BIT_SDR2HDR = 1 << DRM_PLANE_FEARURE_SDR2HDR,
      DRM_PLANE_FEARURE_BIT_AFBDC   = 1 << DRM_PLANE_FEARURE_AFBDC,
};

class DrmDevice;

class DrmPlane {
 public:
  DrmPlane(DrmDevice *drm, drmModePlanePtr p);
  DrmPlane(const DrmPlane &) = delete;
  DrmPlane &operator=(const DrmPlane &) = delete;

  int Init();

  uint32_t id() const;

  bool GetCrtcSupported(const DrmCrtc &crtc) const;

  uint32_t type() const;

  DrmPlaneType win_type() const;

  const DrmProperty &crtc_property() const;
  const DrmProperty &fb_property() const;
  const DrmProperty &crtc_x_property() const;
  const DrmProperty &crtc_y_property() const;
  const DrmProperty &crtc_w_property() const;
  const DrmProperty &crtc_h_property() const;
  const DrmProperty &src_x_property() const;
  const DrmProperty &src_y_property() const;
  const DrmProperty &src_w_property() const;
  const DrmProperty &src_h_property() const;
  const DrmProperty &zpos_property() const;
  const DrmProperty &rotation_property() const;
  const DrmProperty &alpha_property() const;
  const DrmProperty &eotf_property() const;
  const DrmProperty &blend_property() const;
  const DrmProperty &colorspace_property() const;
  const DrmProperty &area_id_property() const;
  const DrmProperty &share_id_property() const;
  const DrmProperty &feature_property() const;
  const DrmProperty &name_property() const;
  const DrmProperty &input_w_property() const;
  const DrmProperty &input_h_property() const;
  const DrmProperty &output_w_property() const;
  const DrmProperty &output_h_property() const;
  const DrmProperty &scale_rate_property() const;
  bool is_use();
  void set_use(bool b_use);
  bool get_scale();
  bool get_rotate();
  bool get_hdr2sdr();
  bool get_sdr2hdr();
  bool get_afbc();
  bool get_afbc_prop();
  bool get_yuv();
  void set_yuv(bool b_yuv);
  bool is_reserved();
  void set_reserved(bool b_reserved);
  bool is_support_scale(float scale_rate);
 inline uint32_t get_possible_crtc_mask() const{ return possible_crtc_mask_; }

 private:
  DrmDevice *drm_;
  uint32_t id_;

  uint32_t possible_crtc_mask_;

  uint32_t type_;

  DrmProperty crtc_property_;
  DrmProperty fb_property_;
  DrmProperty crtc_x_property_;
  DrmProperty crtc_y_property_;
  DrmProperty crtc_w_property_;
  DrmProperty crtc_h_property_;
  DrmProperty src_x_property_;
  DrmProperty src_y_property_;
  DrmProperty src_w_property_;
  DrmProperty src_h_property_;
  DrmProperty rotation_property_;
  DrmProperty alpha_property_;
  DrmProperty blend_mode_property_;
  DrmProperty zpos_property_;
  //RK support
  DrmProperty eotf_property_;
  DrmProperty colorspace_property_;
  DrmProperty area_id_property_;
  DrmProperty share_id_property_;
  DrmProperty feature_property_;
  DrmProperty name_property_;
  DrmProperty input_w_property_;
  DrmProperty input_h_property_;
  DrmProperty output_w_property_;
  DrmProperty output_h_property_;
  DrmProperty scale_rate_property_;

  bool b_reserved_;
  bool b_use_;
  bool b_yuv_;
  bool b_scale_;
  bool b_alpha_;
  bool b_hdr2sdr_;
  bool b_sdr2hdr_;
  bool b_afbdc_;
  bool b_afbc_prop_;
  DrmPlaneType win_type_;
  uint32_t rotate_;
  float scale_min_=0.0;
  float scale_max_=0.0;
};
}  // namespace android

#endif  // ANDROID_DRM_PLANE_H_