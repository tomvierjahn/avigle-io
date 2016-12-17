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

#ifndef AVIGLE__IO__NVM_READER_H_
#define AVIGLE__IO__NVM_READER_H_


#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <io/input_adapter_interface.h>
#include <io/io_api.h>
#include <io/reader_tools.h>


namespace io
{

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
class IO_API NvmReader
{
  friend class InputData;

private:
  enum NvmVersion
  {
    kNvmVersion030 = 3,
    kNvmVersionInvalid
  };

  NvmReader(const std::string& fileName);
  ~NvmReader();

  template <typename FloatType>
  void Load(InputAdapterInterface<FloatType>* pInputAdapter);

  static void GetJpegSize(const std::string& fileName,
                          unsigned int* width,
                          unsigned int* height);

  boost::filesystem::path fInputPath;
  NvmVersion fVersion;
};  // class





////////////////////////////////////////////////////////////////////////////////
/// Loader
////////////////////////////////////////////////////////////////////////////////
template <typename FloatType>
void
NvmReader::Load
(InputAdapterInterface<FloatType>* pInputAdapter)
{
  typedef std::pair<FloatType, FloatType> TextureCentre;

  namespace bf = boost::filesystem;
  namespace iort = io::ReaderTools;

  // open
  std::ifstream inputStream(this->fInputPath.c_str());
  if (!inputStream.is_open())
  {
    std::cerr << "Could not open input file " << this->fInputPath << "!";
    std::cerr << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }

  // determine version
  const std::string version(iort::Line<std::string>(inputStream).substr(0, 6));
  if(version.compare("NVM_V3") == 0)
  {
    this->fVersion = io::NvmReader::kNvmVersion030;
  }
  else
  {
    std::cerr << "Input file " << this->fInputPath << " not a valid NVM file!";
    std::cerr << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }


  // TEXTURES
  std::map<unsigned int, TextureCentre> textureCentres;
  const unsigned int numOfTextures = iort::Line<unsigned int>(inputStream);
  for (unsigned int texNum = 0; texNum < numOfTextures; ++texNum)
  {
    iort::Tokens textureTokens(iort::Line(inputStream, " \t"));

    const boost::filesystem::path textureFile(        // file name
      boost::filesystem::absolute(
        boost::filesystem::path(iort::Token<std::string>(textureTokens)),
        this->fInputPath.parent_path()));

    // compute and store image sizes
    unsigned int jpegWidth = 0u;
    unsigned int jpegHeight = 0u;
    NvmReader::GetJpegSize(textureFile.string(), &jpegWidth, &jpegHeight);
    textureCentres[texNum] =
      TextureCentre(static_cast<FloatType>(0.5) *
                      static_cast<FloatType>(jpegWidth),
                    static_cast<FloatType>(0.5) *
                      static_cast<FloatType>(jpegHeight));

    const FloatType focal = iort::Token<FloatType>(textureTokens); // foc.len.

    const FloatType quatW = iort::Token<FloatType>(textureTokens); // quatern
    const FloatType quatX = iort::Token<FloatType>(textureTokens);
    const FloatType quatY = iort::Token<FloatType>(textureTokens);
    const FloatType quatZ = iort::Token<FloatType>(textureTokens);
    const FloatType posX = iort::Token<FloatType>(textureTokens);  // pos.
    const FloatType posY = iort::Token<FloatType>(textureTokens);
    const FloatType posZ = iort::Token<FloatType>(textureTokens);

    const FloatType quatW2 = quatW * quatW;
    const FloatType quatX2 = quatX * quatX;
    const FloatType quatY2 = quatY * quatY;
    const FloatType quatZ2 = quatZ * quatZ;

    const FloatType m11 = focal * (-quatZ2 - quatY2 + quatX2 + quatW2);
    const FloatType m12 =
      focal * (static_cast<FloatType>(2.0) * quatX * quatY -
               static_cast<FloatType>(2.0) * quatW * quatZ);
    const FloatType m13 =
      focal * (static_cast<FloatType>(2.0) * quatX * quatZ +
               static_cast<FloatType>(2.0) * quatW * quatY);
    const FloatType m21 =
      focal * (static_cast<FloatType>(2.0) * quatW * quatZ +
               static_cast<FloatType>(2.0) * quatX * quatY);
    const FloatType m22 = focal * (-quatZ2 + quatY2 - quatX2 + quatW2);
    const FloatType m23 =
      focal * (static_cast<FloatType>(2.0) * quatY * quatZ -
               static_cast<FloatType>(2.0) * quatW * quatX);
    const FloatType m31 =
      static_cast<FloatType>(2.0) * quatX * quatZ -
      static_cast<FloatType>(2.0) * quatW * quatY;
    const FloatType m32 =
      static_cast<FloatType>(2.0) * quatY * quatZ +
      static_cast<FloatType>(2.0) * quatW * quatX;
    const FloatType m33 = quatZ2 - quatY2 - quatX2 + quatW2;

    const FloatType offsetX =
      focal * (posX * (-quatZ2 - quatY2 + quatX2 + quatW2) +
               posY * (static_cast<FloatType>(2.0) * quatX * quatY -
                     static_cast<FloatType>(2.0) * quatW * quatZ) +
               posZ * (static_cast<FloatType>(2.0) * quatX * quatZ +
                     static_cast<FloatType>(2.0) * quatW * quatY));
    const FloatType offsetY =
      focal * (posX * (static_cast<FloatType>(2.0) * quatW * quatZ +
                     static_cast<FloatType>(2.0) * quatX * quatY) +
               posY * (-quatZ2 + quatY2 - quatX2 + quatW2) +
               posZ * (static_cast<FloatType>(2.0) * quatY * quatZ -
                     static_cast<FloatType>(2.0) * quatW * quatX));
    const FloatType offsetZ =
      posX * (static_cast<FloatType>(2.0) * quatX * quatZ -
            static_cast<FloatType>(2.0) * quatW * quatY) +
      posY * (static_cast<FloatType>(2.0) * quatY * quatZ +
            static_cast<FloatType>(2.0) * quatW * quatX) +
      posZ * (quatZ2 - quatY2 - quatX2 + quatW2);

    pInputAdapter->OnTexture(
      texNum,                                         // id
      textureFile.string(),
      0u,                                             // width
      0u,                                             // height
      posX,                                           // cam pos
      posY,
      posZ,
      2.0f * (quatX * quatZ - quatY * quatW),         // cam dir
      -2.0f * (quatY * quatZ - quatX * quatW),
      1.0f - 2.0f * (quatX * quatX - quatY * quatY),
      m11, m12, m13,                  // matrix for projection
      m21, m22, m23,
      m31, m32, m33,
      offsetX, offsetY, offsetZ,      // offset for reprojection
      textureCentres[texNum].first,   // offset for texture coordinates
      textureCentres[texNum].second
    );
  }

  // POINTS
  const unsigned int numOfPoints = iort::Line<unsigned int>(inputStream);
  for (unsigned int pointNum = 0; pointNum < numOfPoints; ++pointNum)
  {
    iort::Tokens pointTokens(iort::Line(inputStream, " \t"));

    pInputAdapter->OnBeginPoint();
    const FloatType x = iort::Token<FloatType>(pointTokens);
    const FloatType y = iort::Token<FloatType>(pointTokens);
    const FloatType z = iort::Token<FloatType>(pointTokens);
    const FloatType r =
      iort::Token<FloatType>(pointTokens) / static_cast<FloatType>(255.0);
    const FloatType g =
      iort::Token<FloatType>(pointTokens) / static_cast<FloatType>(255.0);
    const FloatType b =
      iort::Token<FloatType>(pointTokens) / static_cast<FloatType>(255.0);

    pInputAdapter->OnPointPosition(x, y, z);
    pInputAdapter->OnPointColour(r, g, b);

    // tex coords per point
    const unsigned int numCoords = iort::Token<unsigned int>(pointTokens);
    for (unsigned int texCoord = 0; texCoord < numCoords; ++texCoord)
    {
      const unsigned int id = iort::Token<unsigned int>(pointTokens);
      iort::Token<iort::Unused>(pointTokens); // feature index --> not yet used
      const FloatType u =
        iort::Token<FloatType>(pointTokens) + textureCentres[id].first;
      const FloatType v =
        iort::Token<FloatType>(pointTokens) + textureCentres[id].second;
      pInputAdapter->OnPointTexCoord(id, u, v);
    }
    pInputAdapter->OnEndPoint();
  }

  if (inputStream.is_open())
  {
    inputStream.close();
  }
}


} // namespace io


#endif  // #ifndef AVIGLE__IO__NVM_READER_H_
