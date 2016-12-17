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

#ifndef AVIGLE__IO__READER_TOOLS_H_
#define AVIGLE__IO__READER_TOOLS_H_

#include <cstdlib>

#include <limits>
#include <string>
#include <utility>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>


namespace io
{

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
namespace ReaderTools
{

typedef void Unused;
typedef boost::char_separator<char> Delimiter;
typedef boost::tokenizer<Delimiter> Tokenizer;
typedef Tokenizer::iterator TokenIter;
struct Tokens
{
  std::string fInput;
  Delimiter   fDelimiters;
  Tokenizer   fTokenizer;
  TokenIter   fTokenIter;
  Tokens(const std::string& input,
         const std::string& delimiters)
  : fInput(input)
  , fDelimiters(delimiters.c_str())
  , fTokenizer(this->fInput, this->fDelimiters)
  , fTokenIter(this->fTokenizer.begin())
  {}
};

template <typename ReturnType>
ReturnType Line(std::ifstream& inputStream);

io::ReaderTools::Tokens Line(std::ifstream& inputStream,
                             const std::string& delimiters);

template <typename ReturnType>
ReturnType Token(io::ReaderTools::Tokens& tokens);

std::string NonCommentLine(std::ifstream& inputStream);




////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
template <>
inline
std::string
Line<std::string>
(std::ifstream& inputStream)
{
  return io::ReaderTools::NonCommentLine(inputStream);
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
template <>
inline
unsigned int
Line<unsigned int>
(std::ifstream& inputStream)
{
  const unsigned int retVal =
    boost::lexical_cast<unsigned int>(
      io::ReaderTools::NonCommentLine(inputStream));
  return retVal;
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
template <typename FloatType>
inline
FloatType
Line
(std::ifstream& inputStream)
{
  const FloatType retVal =
    boost::lexical_cast<FloatType>(
      io::ReaderTools::NonCommentLine(inputStream));
  return retVal;
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
template <>
inline
io::ReaderTools::Unused
Line<io::ReaderTools::Unused>
(std::ifstream& inputStream)
{
  inputStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return;
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
inline
io::ReaderTools::Tokens
Line
(std::ifstream& inputStream,
 const std::string& delimiters)
{
  return Tokens(io::ReaderTools::NonCommentLine(inputStream), delimiters);
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
template <>
inline
std::string
Token<std::string>
(io::ReaderTools::Tokens& tokens)
{
  return *(tokens.fTokenIter++);
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
template <>
inline
unsigned int
Token<unsigned int>
(io::ReaderTools::Tokens& tokens)
{
  const unsigned int retVal =
    boost::lexical_cast<unsigned int>(*(tokens.fTokenIter++));
  return retVal;
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
template <typename FloatType>
inline
FloatType
Token
(io::ReaderTools::Tokens& tokens)
{
  const FloatType retVal =
    boost::lexical_cast<FloatType>(*(tokens.fTokenIter++));
  return retVal;
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
template <>
inline
io::ReaderTools::Unused
Token<io::ReaderTools::Unused>
(io::ReaderTools::Tokens& tokens)
{
  ++tokens.fTokenIter;
  return;
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
inline
std::string
NonCommentLine
(std::ifstream& inputStream)
{
  std::string inputLine;
  do
  {
    std::getline(inputStream, inputLine);
    boost::algorithm::trim_left(inputLine);
  }
  while ((inputLine.size() == 0) || (inputLine.c_str()[0] == '#'));

  return inputLine;
}


} // namespace ReaderTools


} // namespace io


#endif  // AVIGLE__IO__READER_TOOLS_H_
