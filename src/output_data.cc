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

#include <boost/filesystem.hpp>

#include <io/output_data.h>
#include <io/rmv_writer.h>


namespace io
{

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
OutputData::OutputData
(const std::string& fileName)
: fFileType(io::OutputData::kFileTypeInvalid)
, fFileName("")
, fInfo("[none]")
{
  this->Reset(fileName);
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
void
OutputData::Reset
(const std::string& fileName)
{
  namespace bf = boost::filesystem;

  this->fFileName = fileName;

  // create reader based on extension
  const bf::path outputPath(fileName);

  // check if the parent directory exists
  // otherwise create the directories up to the parent directory
  if (!bf::exists(outputPath.parent_path()))
  {
	  bf::create_directories(outputPath.parent_path());
  }

  if (outputPath.has_extension())
  {
	if (outputPath.extension().compare(std::string(".rmv")) == 0)
	{
	  this->fFileType = io::OutputData::kFileTypeRMV;
	}
  }

  // if valid, create info string
  if (this->IsValid())
  {
    const size_t length = fileName.length();
    if (fileName.length() > 40)
    {
      this->fInfo =
        fileName.substr(0,17) + "..." + fileName.substr(length - 20,20);
    }
    else
    {
      this->fInfo = fileName;
    }
  }
}


} // namespace io
