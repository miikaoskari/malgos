#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum PieceType
{
    Original,
    Added
};

struct Piece
{
    PieceType m_type;
    size_t m_start;
    size_t m_length;
};

class PieceTable
{
  public:
    void insert(const std::string &txt, const size_t start);
    void load(const std::string &txt);

  private:
    std::string m_original;
    std::string m_added;
    std::vector<Piece> m_pieces;
};
