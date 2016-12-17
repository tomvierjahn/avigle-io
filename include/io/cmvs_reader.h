//------------------------------------------------------------------------------
// avigle-io -- common io classes/tools
//
// Developed during the research project AVIGLE
// which was part of the Hightech.NRW research program
// funded by the ministry for Innovation, Science, Research and Technology
// of the German state Northrhine-Westfalia, and by the European Union.
//
// Copyright (c) 2010--2013, Tom Vierjahn et al.
//------------------------------------------------------------------------------
//                                License
//
// This library/program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// If you are using this library/program in a project, work or publication,
// please cite [1,2].
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------
//                                References
//
// [1] S. Rohde, N. Goddemeier, C. Wietfeld, F. Steinicke, K. Hinrichs,
//     T. Ostermann, J. Holsten, D. Moormann:
//     "AVIGLE: A System of Systems Concept for an
//      Avionic Digital Service Platform based on
//      Micro Unmanned Aerial Vehicles".
//     In Proc. IEEE Int'l Conf. Systems Man and Cybernetics (SMC),
//     pp. 459--466. 2010. DOI: 10.1109/ICSMC.2010.5641767
// [2] S. Strothoff, D. Feldmann, F. Steinicke, T. Vierjahn, S. Mostafawy:
//     "Interactive generation of virtual environments using MUAVs".
//     In Proc. IEEE Int. Symp. VR Innovations, pp. 89--96, 2011.
//     DOI: 10.1109/ISVRI.2011.5759608
//------------------------------------------------------------------------------

#ifndef AVIGLE__IO__CMVS_READER_H_
#define AVIGLE__IO__CMVS_READER_H_


#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <io/io_api.h>
#include <io/input_adapter_interface.h>
#include <io/reader_tools.h>


namespace io
{

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
class IO_API CmvsReader
{
  friend class InputData;

private:
  CmvsReader(const std::string& fileName);
  ~CmvsReader();

  template <typename FloatType>
  void Load(InputAdapterInterface<FloatType>* pInputAdapter);

  template <class FloatType>
  struct Matrix3x4
  {
    FloatType m[3][4];
    Matrix3x4() {};
    Matrix3x4(FloatType m00, FloatType m01, FloatType m02, FloatType m03,
              FloatType m10, FloatType m11, FloatType m12, FloatType m13,
              FloatType m20, FloatType m21, FloatType m22, FloatType m23)
    {
      m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
      m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
      m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
    }
  };

  boost::filesystem::path fInputPath;
  boost::filesystem::path fCamerasPath;
  boost::filesystem::path fPatchesPath;
  boost::filesystem::path fTextureImagePath;
  boost::filesystem::path fProjectionMatrixFolder;
};  // class





////////////////////////////////////////////////////////////////////////////////
/// Loader
////////////////////////////////////////////////////////////////////////////////
template <typename FloatType>
void
CmvsReader::Load
(InputAdapterInterface<FloatType>* pInputAdapter)
{
  namespace bf = boost::filesystem;
  namespace iort = io::ReaderTools;

  std::map<unsigned int, Matrix3x4<FloatType> > projectionMatrices;

  // TEXTURES
  std::ifstream camerasStream(this->fCamerasPath.c_str());
  if (!camerasStream.is_open())
  {
    std::cerr << "Could not open cameras file!" << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }
  const unsigned int numOfTextures = iort::Line<unsigned int>(camerasStream);
  for (unsigned int texNum = 0; texNum < numOfTextures; ++texNum)
  {
    const bf::path textureFilePath(
      bf::absolute(iort::Line<std::string>(camerasStream),
                   this->fTextureImagePath));

    iort::Line<iort::Unused>(camerasStream);                  // focal length
    iort::Line<iort::Unused>(camerasStream);                  // principal point
    iort::Line<iort::Unused>(camerasStream);                  // translation
    iort::Tokens posTokens(iort::Line(camerasStream, " \t")); // position
    iort::Line<iort::Unused>(camerasStream);                  // axis angle
    iort::Line<iort::Unused>(camerasStream);                  // quaternion
    iort::Line<iort::Unused>(camerasStream);                  // 1st row of 3x3 matrix
    iort::Line<iort::Unused>(camerasStream);                  // 2nd row of 3x3 matrix
    iort::Tokens dirTokens(iort::Line(camerasStream, " \t")); // 3rd: direction

    // determine projection matrix
    bf::path projectionMatrixFile(textureFilePath.stem());
    projectionMatrixFile += ".txt";
    projectionMatrixFile =
    bf::absolute(projectionMatrixFile, this->fProjectionMatrixFolder);

    std::ifstream projectionMatrixStream(projectionMatrixFile.c_str());
    if (!projectionMatrixStream.is_open())
    {
      std::cerr << "Could not open projection matrix file!" << std::endl;
      std::cerr << "Terminating." << std::endl;
      exit(EXIT_FAILURE);
    }

    const std::string magic(iort::Line<std::string>(projectionMatrixStream));
    if (magic.compare("CONTOUR") != 0)
    {
      std::cerr << "Invalid projection matrix file!" << std::endl;
      std::cerr << "Terminating." << std::endl;
      exit(EXIT_FAILURE);
    }

    iort::Tokens fstRowTokens(iort::Line(projectionMatrixStream, " \t"));
    iort::Tokens sndRowTokens(iort::Line(projectionMatrixStream, " \t"));
    iort::Tokens trdRowTokens(iort::Line(projectionMatrixStream, " \t"));

    projectionMatrices[texNum] =
    Matrix3x4<FloatType>(iort::Token<FloatType>(fstRowTokens),
                         iort::Token<FloatType>(fstRowTokens),
                         iort::Token<FloatType>(fstRowTokens),
                         iort::Token<FloatType>(fstRowTokens),
                         iort::Token<FloatType>(sndRowTokens),
                         iort::Token<FloatType>(sndRowTokens),
                         iort::Token<FloatType>(sndRowTokens),
                         iort::Token<FloatType>(sndRowTokens),
                         iort::Token<FloatType>(trdRowTokens),
                         iort::Token<FloatType>(trdRowTokens),
                         iort::Token<FloatType>(trdRowTokens),
                         iort::Token<FloatType>(trdRowTokens));
    if (projectionMatrixStream.is_open())
    {
      projectionMatrixStream.close();
    }

    // store texture
    pInputAdapter->OnTexture(texNum,
                             textureFilePath.string(),
                             0u, 0u,
                             iort::Token<FloatType>(posTokens),
                             iort::Token<FloatType>(posTokens),
                             iort::Token<FloatType>(posTokens),
                             iort::Token<FloatType>(dirTokens),
                             iort::Token<FloatType>(dirTokens),
                             iort::Token<FloatType>(dirTokens),
                             projectionMatrices[texNum].m[0][0],
                             projectionMatrices[texNum].m[0][1],
                             projectionMatrices[texNum].m[0][2],
                             projectionMatrices[texNum].m[1][0],
                             projectionMatrices[texNum].m[1][1],
                             projectionMatrices[texNum].m[1][2],
                             projectionMatrices[texNum].m[2][0],
                             projectionMatrices[texNum].m[2][1],
                             projectionMatrices[texNum].m[2][2],
                             -projectionMatrices[texNum].m[0][3],
                             -projectionMatrices[texNum].m[1][3],
                             -projectionMatrices[texNum].m[2][3],
                             static_cast<FloatType>(0.0),
                             static_cast<FloatType>(0.0));



  } // for camera
  if (camerasStream.is_open())
  {
    camerasStream.close();
  }


  // POINTS
  bf::directory_iterator patchesDirIter(this->fPatchesPath);
  bf::directory_iterator patchesDirEnd;
  for (; patchesDirIter != patchesDirEnd; ++patchesDirIter)
  {
    if (patchesDirIter->path().extension().compare(std::string(".patch")) == 0)
    {
      // get accompanying ply file
      bf::path pointsFile(patchesDirIter->path());
      pointsFile.replace_extension(std::string(".ply"));

      std::ifstream patchesStream(patchesDirIter->path().c_str());
      std::ifstream pointsStream(pointsFile.c_str());

      if (!(patchesStream.is_open() && pointsStream.is_open()))
      {
        std::cerr << "Could not open patches files!" << std::endl;
        std::cerr << "Terminating." << std::endl;
        exit(EXIT_FAILURE);
      }

      const std::string magic(iort::Line<std::string>(patchesStream));
      if (magic.compare("PATCHES") != 0)
      {
        std::cerr << "Invalid patches file!" << std::endl;
        std::cerr << "Terminating." << std::endl;
        exit(EXIT_FAILURE);
      }

      const unsigned int numPatches = iort::Line<unsigned int>(patchesStream);

      iort::Line<iort::Unused>(pointsStream);   // ply
      iort::Line<iort::Unused>(pointsStream);   // format ascii #

      iort::Tokens numPointTokens(iort::Line(pointsStream, " \t"));
      iort::Token<iort::Unused>(numPointTokens);  // element
      iort::Token<iort::Unused>(numPointTokens);  // vertex
      unsigned int numPoints =
        iort::Token<unsigned int>(numPointTokens);

      if (numPatches != numPoints)
      {
        std::cerr << "Different numbers of points and patches!" << std::endl;
        std::cerr << "Terminating." << std::endl;
        exit(EXIT_FAILURE);
      }

      // seek beginning point definition in ply file
      while(iort::Line<std::string>(pointsStream).compare("end_header") != 0);

      for (unsigned int pointId = 0; pointId < numPoints; ++pointId)
      {
        iort::Tokens pointTokens(iort::Line(pointsStream, " \t"));

        pInputAdapter->OnBeginPoint();
        const FloatType pointPosX = iort::Token<FloatType>(pointTokens);
        const FloatType pointPosY = iort::Token<FloatType>(pointTokens);
        const FloatType pointPosZ = iort::Token<FloatType>(pointTokens);
        pInputAdapter->OnPointPosition(pointPosX, pointPosY, pointPosZ);

        iort::Token<iort::Unused>(pointTokens); // nx
        iort::Token<iort::Unused>(pointTokens); // ny
        iort::Token<iort::Unused>(pointTokens); // nz

        const FloatType pointColR =
          iort::Token<FloatType>(pointTokens) / static_cast<FloatType>(255.0);
        const FloatType pointColG =
          iort::Token<FloatType>(pointTokens) / static_cast<FloatType>(255.0);;
        const FloatType pointColB =
          iort::Token<FloatType>(pointTokens) / static_cast<FloatType>(255.0);;
        pInputAdapter->OnPointColour(pointColR, pointColG, pointColB);

        // seek next patch in patch file
        while(iort::Line<std::string>(patchesStream).compare("PATCHS") != 0);

        iort::Line<iort::Unused>(patchesStream); // position
        iort::Line<iort::Unused>(patchesStream); // normal
        iort::Line<iort::Unused>(patchesStream); // confidence and debug

        const unsigned int numCoords = iort::Line<unsigned int>(patchesStream);
        iort::Tokens textures(iort::Line(patchesStream, " \t"));
        for (unsigned int texCoord = 0u; texCoord < numCoords; ++texCoord)
        {
          const unsigned int textureId = iort::Token<unsigned int>(textures);
          const Matrix3x4<FloatType> projection = projectionMatrices[textureId];

          const FloatType depth =
            projection.m[2][0] * pointPosX +
            projection.m[2][1] * pointPosY +
            projection.m[2][2] * pointPosZ +
            projection.m[2][3];
          const FloatType u =
            (projection.m[0][0] * pointPosX +
             projection.m[0][1] * pointPosY +
             projection.m[0][2] * pointPosZ +
             projection.m[0][3]) / depth;

          const FloatType v =
            (projection.m[1][0] * pointPosX +
             projection.m[1][1] * pointPosY +
             projection.m[1][2] * pointPosZ +
             projection.m[1][3]) / depth;

          pInputAdapter->OnPointTexCoord(textureId, u, v);
        } // for all tex coords per point
        pInputAdapter->OnEndPoint();
      } // for all points
      if (patchesStream.is_open())
      {
        patchesStream.close();
      }
      if (pointsStream.is_open())
      {
        pointsStream.close();
      }
    } // if .patch file
  } // for all .patch files
}


} // namespace io


#endif  // #ifdef AVIGLE__IO__RMV_READER_H_
