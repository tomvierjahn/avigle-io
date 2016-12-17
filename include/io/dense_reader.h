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

#ifndef AVIGLE__IO__DENSE_READER_H_
#define AVIGLE__IO__DENSE_READER_H_


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
class IO_API DenseReader
{
  friend class InputData;

private:
  DenseReader(const std::string& fileName);
  ~DenseReader();

  template <typename FloatType>
  void Load(InputAdapterInterface<FloatType>* pInputAdapter);

  boost::filesystem::path fInputPath;
};  // class





////////////////////////////////////////////////////////////////////////////////
/// Loader
////////////////////////////////////////////////////////////////////////////////
template <typename FloatType>
void
DenseReader::Load
(InputAdapterInterface<FloatType>* pInputAdapter)
{
  namespace bf = boost::filesystem;
  namespace iort = io::ReaderTools;

  // POINTS
  bf::directory_iterator patchesDirIter(this->fInputPath);
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

          pInputAdapter->OnPointTexCoord(textureId, 0.0f, 0.0f);
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


#endif  // #ifdef AVIGLE__IO__DENSE_READER_H_
