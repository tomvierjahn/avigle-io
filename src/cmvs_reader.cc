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

#include <cstdlib>

#include <boost/filesystem.hpp>

#include <io/cmvs_reader.h>


namespace io
{

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
CmvsReader::CmvsReader
(const std::string& fileName)
: fInputPath(fileName)
{
  namespace bf = boost::filesystem;

  // check if exists
  if (!bf::exists(this->fInputPath))
  {
    std::cerr << "Input directory " << this->fInputPath << " does not exists!" << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }

  // determine cameras path
  this->fCamerasPath = bf::absolute("00", this->fInputPath);
  this->fCamerasPath /= "cameras.txt";
  if (!bf::exists(this->fCamerasPath))
  {
    std::cerr << "Did not find cameras file " << this->fCamerasPath << "!" << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }

  // determine patches path
  this->fPatchesPath = bf::absolute("00", this->fInputPath);
  this->fPatchesPath /= "models";
  if (!bf::exists(this->fPatchesPath))
  {
    std::cerr << "Did not find patches path " << this->fPatchesPath << "!" << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }

  // detemine texture images path
  this->fTextureImagePath = bf::absolute("00", this->fInputPath);
  this->fTextureImagePath /= "visualize";
  if (!bf::exists(this->fTextureImagePath))
  {
    std::cerr << "Did not find texture images path " << this->fTextureImagePath << "!" << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }

  // detemine projection matrix folder
  this->fProjectionMatrixFolder = bf::absolute("00", this->fInputPath);
  this->fProjectionMatrixFolder /= "txt";
  if (!bf::exists(this->fProjectionMatrixFolder))
  {
    std::cerr << "Did not find projection matrix folder "
      << this->fProjectionMatrixFolder << "!" << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
CmvsReader::~CmvsReader
()
{
}


} // namespace io
