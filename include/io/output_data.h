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

#ifndef AVIGLE__IO__OUTPUT_DATA_H_
#define AVIGLE__IO__OUTPUT_DATA_H_


#include <cstdlib>

#include <string>

#include <io/io_api.h>
#include <io/rmv_writer.h>


namespace io
{

// forward declarations
template <typename FloatType> class OutputAdapterInterface;

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
class IO_API OutputData
{
public:
  enum FileType
  {
    kFileTypeRMV = 0,
    kFileTypeInvalid
  };

  OutputData(const std::string& fileName = std::string(""));
  void Reset(const std::string& fileName = std::string(""));

  template <typename FloatType>
  void Write(io::OutputAdapterInterface<FloatType>* pOutputAdapter);

  bool IsValid() const { return (this->fFileType != kFileTypeInvalid); }
  const std::string& GetInfo() const { return this->fInfo; }

private:
  FileType fFileType;

  std::string fFileName;
  std::string fInfo;
};

} // namespace io





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
template <typename FloatType>
void
io::OutputData::Write
(io::OutputAdapterInterface<FloatType>* pOutputAdapter)
{
  if (this->fFileType == kFileTypeRMV)
  {
    RmvWriter writer(this->fFileName);
    writer.Write(pOutputAdapter);
  }
  else
  {
    std::cerr << "Invalid file!" << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }
}


#endif  // #ifndef AVIGLE__IO__INPUT_DATA_H_
