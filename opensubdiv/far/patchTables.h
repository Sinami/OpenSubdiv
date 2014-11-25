//
//   Copyright 2013 Pixar
//
//   Licensed under the Apache License, Version 2.0 (the "Apache License")
//   with the following modification; you may not use this file except in
//   compliance with the Apache License and the following modification to it:
//   Section 6. Trademarks. is deleted and replaced with:
//
//   6. Trademarks. This License does not grant permission to use the trade
//      names, trademarks, service marks, or product names of the Licensor
//      and its affiliates, except as required to comply with Section 4(c) of
//      the License and to reproduce the content of the NOTICE file.
//
//   You may obtain a copy of the Apache License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the Apache License with the above modification is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//   KIND, either express or implied. See the Apache License for the specific
//   language governing permissions and limitations under the Apache License.
//

#ifndef FAR_PATCH_TABLES_H
#define FAR_PATCH_TABLES_H

#include "../version.h"

#include "../far/patchDescriptor.h"
#include "../far/patchParam.h"
#include "../far/stencilTables.h"
#include "../far/types.h"

#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <vector>
#include <map>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

namespace Far {

/// \brief Container for arrays of parametric patches
///
/// PatchTables contain topology and parametric information about the patches
/// generated by the Refinement process. Patches in the tables are sorted into
/// arrays based on their PatchDescriptor Type.
///
class PatchTables {

public:

    /// \brief Handle that can be used as unique patch identifier within PatchTables
    class PatchHandle {
    // XXXX manuelk members will eventually be private once FVar
    //              interpolation is implemented fully
    public:

        friend class PatchTables;
        friend class PatchMap;

        Index arrayIndex, // Array index of the patch
              patchIndex, // Absolute Index of the patch
              vertIndex;  // Relative offset to the first CV of the patch in array
    };

public:

    /// \brief Copy constructor
    PatchTables(PatchTables const & src);

    /// \brief Destructor
    ~PatchTables();

    /// \brief True if the patches are of feature adaptive types
    bool IsFeatureAdaptive() const;

    /// \brief Returns the total number of control vertex indices in the tables
    int GetNumControlVerticesTotal() const {
        return (int)_patchVerts.size();
    }

    /// \brief Returns the total number of patches stored in the tables
    int GetNumPatchesTotal() const;

    /// \brief Returns max vertex valence
    int GetMaxValence() const { return _maxValence; }

    /// \brief Returns the total number of ptex faces in the mesh
    int GetNumPtexFaces() const { return _numPtexFaces; }


    //@{
    ///  @name Direct accessors
    ///
    /// \warning These direct accessors are left for convenience, but they are
    ///          likely going to be deprecated in future releases
    ///

    typedef std::vector<Index> PatchVertsTable;

    /// \brief Get the table of patch control vertices
    PatchVertsTable const & GetPatchControlVerticesTable() const { return _patchVerts; }

    /// \brief Returns the PatchParamTable (PatchParams order matches patch array sorting)
    PatchParamTable const & GetPatchParamTable() const { return _paramTable; }

    /// \brief Returns a sharpness index table for each patch (if exists)
    std::vector<Index> const &GetSharpnessIndexTable() const { return _sharpnessIndices; }

    /// \brief Returns sharpness values table
    std::vector<float> const &GetSharpnessValues() const { return _sharpnessValues; }

    typedef std::vector<unsigned int> QuadOffsetsTable;

    /// \brief Returns the quad-offsets table
    QuadOffsetsTable const & GetQuadOffsetsTable() const {
        return _quadOffsetsTable;
    }
    //@}

    //@{
    ///  @name Individual patches
    ///
    /// \anchor individual_patches
    ///
    /// \brief Accessors for individual patches
    ///

    /// \brief Returns the PatchDescriptor for the patches in array 'array'
    PatchDescriptor GetPatchDescriptor(PatchHandle const & handle) const;

    /// \brief Returns the control vertex indices for the patch identified by 'handle'
    IndexArray const GetPatchVertices(PatchHandle const & handle) const;

    /// \brief Returns a PatchParam for the patch identified by 'handle'
    PatchParam GetPatchParam(PatchHandle const & handle) const;

    /// \brief Returns the control vertex indices for the patch 'patch' in array 'array'
    IndexArray const GetPatchVertices(int array, int patch) const;

    /// \brief Returns the PatchParam for the patch 'patch' in array 'array'
    PatchParam GetPatchParam(int array, int patch) const;
    //@}


    //@{
    ///  @name Arrays of patches
    ///
    /// \anchor arrays_of_patches
    ///
    /// \brief Accessors for arrays of patches of the same type
    ///

    /// \brief Returns the number of patch arrays in the table
    int GetNumPatchArrays() const;

    /// \brief Returns the number of patches in patch array 'array'
    int GetNumPatches(int array) const;

    /// \brief Returns the number of control vertices in patch array 'array'
    int GetNumControlVertices(int array) const;

    /// \brief Returns the PatchDescriptor for the patches in array 'array'
    PatchDescriptor GetPatchArrayDescriptor(int array) const;

    /// \brief Returns the control vertex indices for the patches in array 'array'
    IndexArray const GetPatchArrayVertices(int array) const;

    /// \brief Returns the PatchParams for the patches in array 'array'
    PatchParamArray const GetPatchParams(int array) const;
    //@}


    //@{
    ///  @name End-Cap patches
    ///
    /// \anchor end_cap_patches
    ///
    /// \brief Accessors for end-cap patch additional data
    ///

    typedef Vtr::Array<unsigned int> QuadOffsetsArray;

    /// \brief Returns the 'QuadOffsets' for the Gregory patch identified by 'handle'
    QuadOffsetsArray const GetPatchQuadOffsets(PatchHandle const & handle) const;

    typedef std::vector<Index> VertexValenceTable;

    /// \brief Returns the 'VertexValences' table (vertex neighborhoods table)
    VertexValenceTable const & GetVertexValenceTable() const {
        return _vertexValenceTable;
    }

    /// \brief Returns a stencil table for the control vertices of end-cap patches
    StencilTables const * GetEndCapStencilTables() const { return _endcapStencilTables; }

    Index GetEndCapStencilIndex(PatchHandle const & handle) const {
        return handle.vertIndex;
    }
    //@}


    //@{
    ///  @name Single-crease patches
    ///
    /// \anchor single_crease_patches
    ///
    /// \brief Accessors for single-crease patch edge sharpness
    ///

    /// \brief Returns the crease sharpness for the patch identified by 'handle'
    ///        if it is a single-crease patch, or 0.0f
    float GetSingleCreasePatchSharpnessValue(PatchHandle const & handle) const;

    /// \brief Returns the crease sharpness for the patch 'patch' in array 'array'
    ///        if it is a single-crease patch, or 0.0f
    float GetSingleCreasePatchSharpnessValue(int array, int patch) const;
    //@}

    //@{
    ///  @name Face-varying
    ///
    ///  \anchor face_varying
    ///
    /// \brief Face-varying patch vertex indices tables
    ///
    /// FVarPatchTables contain the topology for face-varying primvar data
    /// channels. The patch ordering matches that of PatchTables PatchArrays.
    ///
    /// \note bi-cubic face-varying limit interpolation is not implemented yet :
    ///       this code will change soon
    ///
    class FVarPatchTables {

    public:

        /// \brief Returns the number of face-varying primvar channels
        int GetNumChannels() const {
            return (int)_channels.size();
        }

        /// \brief Returns the face-varying patches vertex indices
        ///
        /// @param channel  Then face-varying primvar channel index
        ///
        std::vector<Index> const & GetPatchVertices(int channel) const {
            return _channels[channel].patchVertIndices;
        }

    private:
        friend class PatchTables;
        friend class PatchTablesFactory;

        struct Channel {
            friend class PatchTablesFactory;

            std::vector<Index> patchVertIndices; // face-varying vertex indices
        };

    private:

        std::vector<Channel> _channels; // face-varying primvar channels
    };

    /// \brief Returns the face-varying patches
    FVarPatchTables const * GetFVarPatchTables() const { return _fvarPatchTables; }
    //@}

public:

    //
    // Interpolation methods
    //

    /// \brief Interpolate the (s,t) parametric location of a *bilinear* patch
    ///
    /// \note This method can only be used on uniform PatchTables of quads (see
    ///       IsFeatureAdaptive() method)
    ///
    /// @param handle  A patch handle indentifying the sub-patch containing the
    ///                (s,t) location
    ///
    /// @param s       Patch coordinate (in coarse face normalized space)
    ///
    /// @param t       Patch coordinate (in coarse face normalized space)
    ///
    /// @param src     Source primvar buffer (control vertices data)
    ///
    /// @param dst     Destination primvar buffer (limit surface data)
    ///
    template <class T, class U> void Interpolate(PatchHandle const & handle,
        float s, float t, T const & src, U & dst) const;

    /// \brief Interpolate the (s,t) parametric location of a bilinear (quad)
    /// patch
    ///
    template <class T, class U> static void
    InterpolateBilinear(Index const * cvs, float s, float t,
        T const & src, U & dst);

    /// \brief Interpolate the (s,t) parametric location of a regular bicubic
    ///        patch
    ///
    /// @param cvs     Array of 16 control vertex indices
    ///
    /// @param Q       Array of 16 bicubic weights for the control vertices
    ///
    /// @param Qd1     Array of 16 bicubic 's' tangent weights for the control
    ///                vertices
    ///
    /// @param Qd2     Array of 16 bicubic 't' tangent weights for the control
    ///                vertices
    ///
    /// @param src     Source primvar buffer (control vertices data)
    ///
    /// @param dst     Destination primvar buffer (limit surface data)
    ///
    template <class T, class U> static void
    InterpolateRegularPatch(Index const * cvs,
        float const * Q, float const *Qd1, float const *Qd2, T const & src, U & dst);

    /// \brief Interpolate the (s,t) parametric location of a boundary bicubic
    ///        patch
    ///
    /// @param cvs     Array of 12 control vertex indices
    ///
    /// @param Q       Array of 12 bicubic weights for the control vertices
    ///
    /// @param Qd1     Array of 12 bicubic 's' tangent weights for the control
    ///                vertices
    ///
    /// @param Qd2     Array of 12 bicubic 't' tangent weights for the control
    ///                vertices
    ///
    /// @param src     Source primvar buffer (control vertices data)
    ///
    /// @param dst     Destination primvar buffer (limit surface data)
    ///
    template <class T, class U> static void
    InterpolateBoundaryPatch(Index const * cvs,
        float const * Q, float const *Qd1, float const *Qd2, T const & src, U & dst);

    /// \brief Interpolate the (s,t) parametric location of a corner bicubic
    ///        patch
    ///
    /// @param cvs     Array of 9 control vertex indices
    ///
    /// @param Q       Array of 9 bicubic weights for the control vertices
    ///
    /// @param Qd1     Array of 9 bicubic 's' tangent weights for the control
    ///                vertices
    ///
    /// @param Qd2     Array of 9 bicubic 't' tangent weights for the control
    ///                vertices
    ///
    /// @param src     Source primvar buffer (control vertices data)
    ///
    /// @param dst     Destination primvar buffer (limit surface data)
    ///
    template <class T, class U> static void
    InterpolateCornerPatch(Index const * cvs,
        float const * Q, float const *Qd1, float const *Qd2, T const & src, U & dst);

    /// \brief Interpolate the (s,t) parametric location of a Gregory bicubic
    ///        patch
    ///
    /// @param basisStencils  Stencil tables driving the 20 CV basis of the patches
    ///
    /// @param stencilIndex   Index of the first CV stencil in the basis stencils tables
    ///
    /// @param s              Patch coordinate (in coarse face normalized space)
    ///
    /// @param t              Patch coordinate (in coarse face normalized space)
    ///
    /// @param Q              Array of 9 bicubic weights for the control vertices
    ///
    /// @param Qd1            Array of 9 bicubic 's' tangent weights for the control
    ///                       vertices
    ///
    /// @param Qd2            Array of 9 bicubic 't' tangent weights for the control
    ///                       vertices
    ///
    /// @param src            Source primvar buffer (control vertices data)
    ///
    /// @param dst            Destination primvar buffer (limit surface data)
    ///
    template <class T, class U> static void
    InterpolateGregoryPatch(StencilTables const * basisStencils, int stencilIndex,
        float s, float t, float const * Q, float const *Qd1, float const *Qd2,
            T const & src, U & dst);

    /// \brief Interpolate the (s,t) parametric location of a *bicubic* patch
    ///
    /// \note This method can only be used on feature adaptive PatchTables (ie.
    ///       IsFeatureAdaptive() is false)
    ///
    /// @param handle  A patch handle indentifying the sub-patch containing the
    ///                (s,t) location
    ///
    /// @param s       Patch coordinate (in coarse face normalized space)
    ///
    /// @param t       Patch coordinate (in coarse face normalized space)
    ///
    /// @param src     Source primvar buffer (control vertices data)
    ///
    /// @param dst     Destination primvar buffer (limit surface data)
    ///
    template <class T, class U> void Limit(PatchHandle const & handle,
        float s, float t, T const & src, U & dst) const;

protected:

    friend class PatchTablesFactory;

    // Factory constructor
    PatchTables(int maxvalence);

    enum TensorBasis {
        BASIS_BEZIER,
        BASIS_BSPLINE
    };

    // Returns bi-cubic interpolation coefficients for a given (s,t) location
    // on a b-spline patch
    static void getBasisWeightsAtUV(TensorBasis basis, PatchParam::BitField bits,
        float s, float t, float point[16], float deriv1[16], float deriv2[16]);

protected:

    void reservePatchArrays(int numPatchArrays);

    void pushPatchArray(PatchDescriptor desc,
        int npatches, Index * vidx, Index * pidx, Index * qoidx=0);

    Index findPatchArray(PatchDescriptor desc);

    IndexArray getPatchArrayVertices(int arrayIndex);
    PatchParamArray getPatchParams(int arrayIndex);

    Index * getSharpnessIndices(Index arrayIndex);
    float * getSharpnessValues(Index arrayIndex);

    IndexArray getFVarVerts(int arrayIndex, int channel);

private:

    //
    // Patch arrays
    //

    struct PatchArray;

    PatchArray & getPatchArray(Index arrayIndex);
    PatchArray const & getPatchArray(Index arrayIndex) const;

private:

    typedef std::vector<PatchArray> PatchArrayVector;

    //
    // Topology
    //

    int _maxValence,   // highest vertex valence found in the mesh
        _numPtexFaces; // total number of ptex faces

    PatchArrayVector     _patchArrays;  // Vector of descriptors for arrays of patches

    std::vector<Index>   _patchVerts;   // Indices of the control vertices of the patches

    PatchParamTable      _paramTable;   // PatchParam bitfields (one per patch)

    //
    // Extraordinary vertex closed-form evaluation
    //

    // XXXX manuelk end-cap stencils will obsolete the other tables

    StencilTables const * _endcapStencilTables;
#ifdef ENDCAP_TOPOPOLGY
    std::vector<Index>    _endcapTopology;
#endif
    QuadOffsetsTable     _quadOffsetsTable;   // Quad offsets (for Gregory patches)
    VertexValenceTable   _vertexValenceTable; // Vertex valence table (for Gregory patches)

    //
    // Face-varying data
    //

    FVarPatchTables * _fvarPatchTables; // sparse face-varying patch table (one per patch)

    //
    // 'single-crease' patch sharpness tables
    //

    std::vector<Index>   _sharpnessIndices; // Indices of single-crease sharpness (one per patch)
    std::vector<float>   _sharpnessValues;  // Sharpness values.
};

template <class T, class U>
inline void
PatchTables::InterpolateBilinear(Index const * cvs, float s, float t,
    T const & src, U & dst) {

    float os = 1.0f - s,
          ot = 1.0f - t,
            Q[4] = { os*ot,  s*ot, s*t, os*t },
          dQ1[4] = { t-1.0f,   ot,   t,   -t },
          dQ2[4] = { s-1.0f,   -s,   s,   os };

    for (int k=0; k<4; ++k) {
        dst.AddWithWeight(src[cvs[k]], Q[k], dQ1[k], dQ2[k]);
    }
}


template <class T, class U>
inline void
PatchTables::InterpolateRegularPatch(Index const * cvs,
    float const * Q, float const *Qd1, float const *Qd2,
        T const & src, U & dst) {

    //
    //  v0 -- v1 -- v2 -- v3
    //   |.....|.....|.....|
    //   |.....|.....|.....|
    //  v4 -- v5 -- v6 -- v7
    //   |.....|.....|.....|
    //   |.....|.....|.....|
    //  v8 -- v9 -- v10-- v11
    //   |.....|.....|.....|
    //   |.....|.....|.....|
    //  v12-- v13-- v14-- v15
    //
    for (int k=0; k<16; ++k) {
        dst.AddWithWeight(src[cvs[k]], Q[k], Qd1[k], Qd2[k]);
    }
}

template <class T, class U>
inline void
PatchTables::InterpolateBoundaryPatch(Index const * cvs,
    float const * Q, float const *Qd1, float const *Qd2,
        T const & src, U & dst) {

    // mirror the missing vertices (M)
    //
    //  M0 -- M1 -- M2 -- M3 (corner)
    //   |     |     |     |
    //   |     |     |     |
    //  v0 -- v1 -- v2 -- v3    M : mirrored
    //   |.....|.....|.....|
    //   |.....|.....|.....|
    //  v4 -- v5 -- v6 -- v7    v : original Cv
    //   |.....|.....|.....|
    //   |.....|.....|.....|
    //  v8 -- v9 -- v10-- v11
    //
    for (int k=0; k<4; ++k) { // M0 - M3
        dst.AddWithWeight(src[cvs[k]],    2.0f*Q[k],  2.0f*Qd1[k],  2.0f*Qd2[k]);
        dst.AddWithWeight(src[cvs[k+4]], -1.0f*Q[k], -1.0f*Qd1[k], -1.0f*Qd2[k]);
    }
    for (int k=0; k<12; ++k) {
        dst.AddWithWeight(src[cvs[k]], Q[k+4], Qd1[k+4], Qd2[k+4]);
    }
}

template <class T, class U>
inline void
PatchTables::InterpolateCornerPatch(Index const * cvs,
    float const * Q, float const *Qd1, float const *Qd2,
        T const & src, U & dst) {

    // mirror the missing vertices (M)
    //
    //  M0 -- M1 -- M2 -- M3 (corner)
    //   |     |     |     |
    //   |     |     |     |
    //  v0 -- v1 -- v2 -- M4    M : mirrored
    //   |.....|.....|     |
    //   |.....|.....|     |
    //  v3.--.v4.--.v5 -- M5    v : original Cv
    //   |.....|.....|     |
    //   |.....|.....|     |
    //  v6 -- v7 -- v8 -- M6
    //
    for (int k=0; k<3; ++k) { // M0 - M2
        dst.AddWithWeight(src[cvs[k  ]],  2.0f*Q[k],  2.0f*Qd1[k],  2.0f*Qd2[k]);
        dst.AddWithWeight(src[cvs[k+3]], -1.0f*Q[k], -1.0f*Qd1[k], -1.0f*Qd2[k]);
    }
    for (int k=0; k<3; ++k) { // M4 - M6
        int idx = (k+1)*4 + 3;
        dst.AddWithWeight(src[cvs[k*3+2]],  2.0f*Q[idx],  2.0f*Qd1[idx],  2.0f*Qd2[idx]);
        dst.AddWithWeight(src[cvs[k*3+1]], -1.0f*Q[idx], -1.0f*Qd1[idx], -1.0f*Qd2[idx]);
    }
    // M3 = -2.v1 + 4.v2 + v4 - 2.v5
    dst.AddWithWeight(src[cvs[1]], -2.0f*Q[3], -2.0f*Qd1[3], -2.0f*Qd2[3]);
    dst.AddWithWeight(src[cvs[2]],  4.0f*Q[3],  4.0f*Qd1[3],  4.0f*Qd2[3]);
    dst.AddWithWeight(src[cvs[4]],  1.0f*Q[3],  1.0f*Qd1[3],  1.0f*Qd2[3]);
    dst.AddWithWeight(src[cvs[5]], -2.0f*Q[3], -2.0f*Qd1[3], -2.0f*Qd2[3]);
    for (int y=0; y<3; ++y) { // v0 - v8
        for (int x=0; x<3; ++x) {
            int idx = y*4+x+4;
            dst.AddWithWeight(src[cvs[y*3+x]], Q[idx], Qd1[idx], Qd2[idx]);
        }
    }
}

template <class T, class U>
inline void
PatchTables::InterpolateGregoryPatch(StencilTables const * basisStencils,
    int stencilIndex, float s, float t,
        float const * Q, float const *Qd1, float const *Qd2,
            T const & src, U & dst) {

    float ss = 1-s,
          tt = 1-t;
// remark #1572: floating-point equality and inequality comparisons are unreliable
#ifdef __INTEL_COMPILER
#pragma warning disable 1572
#endif
    float d11 = s+t;   if(s+t==0.0f)   d11 = 1.0f;
    float d12 = ss+t;  if(ss+t==0.0f)  d12 = 1.0f;
    float d21 = s+tt;  if(s+tt==0.0f)  d21 = 1.0f;
    float d22 = ss+tt; if(ss+tt==0.0f) d22 = 1.0f;
#ifdef __INTEL_COMPILER
#pragma warning enable 1572
#endif

    float weights[4][2] = { {  s/d11,  t/d11 },
                            { ss/d12,  t/d12 },
                            {  s/d21, tt/d21 },
                            { ss/d22, tt/d22 } };

    //
    //  P3         e3-      e2+         P2
    //     O--------O--------O--------O
    //     |        |        |        |
    //     |        |        |        |
    //     |        | f3-    | f2+    |
    //     |        O        O        |
    // e3+ O------O            O------O e2-
    //     |     f3+          f2-     |
    //     |                          |
    //     |                          |
    //     |      f0-         f1+     |
    // e0- O------O            O------O e1+
    //     |        O        O        |
    //     |        | f0+    | f1-    |
    //     |        |        |        |
    //     |        |        |        |
    //     O--------O--------O--------O
    //  P0         e0+      e1-         P1
    //
    // XXXX manuelk re-order stencils in factory and get rid of permutation ?
    int const permute[16] =
        { 0, 1, 7, 5, 2, -1, -1, 6, 16, -1, -1, 12, 15, 17, 11, 10 };

    for (int i=0, fcount=0; i<16; ++i) {

        int index = permute[i],
            offset = stencilIndex;

        if (index==-1) {

            // 0-ring vertex: blend 2 extra basis CVs
            int const fpermute[4][2] = { {3, 4}, {9, 8}, {19, 18}, {13, 14} };

            assert(fcount < 4);
            int v0 = fpermute[fcount][0],
                v1 = fpermute[fcount][1];

            Stencil s0 = basisStencils->GetStencil(offset + v0),
                    s1 = basisStencils->GetStencil(offset + v1);

            float w0=weights[fcount][0],
                  w1=weights[fcount][1];

            {
                Index const * srcIndices = s0.GetVertexIndices();
                float const * srcWeights = s0.GetWeights();
                for (int j=0; j<s0.GetSize(); ++j) {
                    dst.AddWithWeight(src[srcIndices[j]],
                        Q[i]*w0*srcWeights[j], Qd1[i]*w0*srcWeights[j],
                            Qd2[i]*w0*srcWeights[j]);
                }
            }
            {
                Index const * srcIndices = s1.GetVertexIndices();
                float const * srcWeights = s1.GetWeights();
                for (int j=0; j<s1.GetSize(); ++j) {
                    dst.AddWithWeight(src[srcIndices[j]],
                        Q[i]*w1*srcWeights[j], Qd1[i]*w1*srcWeights[j],
                            Qd2[i]*w1*srcWeights[j]);
                }
            }
            ++fcount;
        } else {
            Stencil s = basisStencils->GetStencil(offset + index);
            Index const * srcIndices = s.GetVertexIndices();
            float const * srcWeights = s.GetWeights();
            for (int j=0; j<s.GetSize(); ++j) {
                dst.AddWithWeight( src[srcIndices[j]],
                    Q[i]*srcWeights[j], Qd1[i]*srcWeights[j],
                         Qd2[i]*srcWeights[j]);
            }
        }
    }
}

// Interpolates the limit position of a parametric location on a bilinear patch
template <class T, class U>
inline void
PatchTables::Interpolate(PatchHandle const & handle, float s, float t,
    T const & src, U & dst) const {

    assert(not IsFeatureAdaptive());

    IndexArray cvs = GetPatchVertices(handle);

    PatchParam::BitField const & bits =
        _paramTable[handle.patchIndex].bitField;
    bits.Normalize(s,t);


    dst.Clear();

    InterpolateBilinear(cvs.begin(), s, t, src, dst);
}

// Interpolates the limit position of a parametric location on a patch
template <class T, class U>
inline void
PatchTables::Limit(PatchHandle const & handle, float s, float t,
    T const & src, U & dst) const {

    assert(IsFeatureAdaptive());

    PatchParam::BitField const & bits = _paramTable[handle.patchIndex].bitField;
    bits.Normalize(s,t);

    PatchDescriptor::Type ptype =
        GetPatchArrayDescriptor(handle.arrayIndex).GetType();

    dst.Clear();

    float Q[16], Qd1[16], Qd2[16];

    if (ptype>=PatchDescriptor::REGULAR and ptype<=PatchDescriptor::CORNER) {

        getBasisWeightsAtUV(BASIS_BSPLINE, bits, s, t, Q, Qd1, Qd2);

        IndexArray cvs = GetPatchVertices(handle);

        switch (ptype) {
            case PatchDescriptor::REGULAR:
                InterpolateRegularPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);
                break;
            case PatchDescriptor::SINGLE_CREASE:
                // TODO: implement InterpolateSingleCreasePatch().
                //InterpolateRegularPatch(cvs, Q, Qd1, Qd2, src, dst);
                break;
            case PatchDescriptor::BOUNDARY:
                InterpolateBoundaryPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);
                break;
            case PatchDescriptor::CORNER:
                InterpolateCornerPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);
                break;
            case PatchDescriptor::GREGORY:
            case PatchDescriptor::GREGORY_BOUNDARY:
                assert(0);
                break;
            default:
                assert(0);
        }
    } else if (ptype==PatchDescriptor::GREGORY_BASIS) {

        assert(_endcapStencilTables);

        getBasisWeightsAtUV(BASIS_BEZIER, bits, s, t, Q, Qd1, Qd2);

        InterpolateGregoryPatch(_endcapStencilTables, handle.vertIndex,
            s, t, Q, Qd1, Qd2, src, dst);

    } else {
        assert(0);
    }
}

} // end namespace Far

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

} // end namespace OpenSubdiv

#endif /* FAR_PATCH_TABLES */
