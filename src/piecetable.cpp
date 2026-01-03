#include "malgos/piecetable.hpp"
#include <cstddef>
#include <string>

void PieceTable::load(const std::string &txt)
{
    m_original = txt;
    Piece pc = {Original, 0, txt.length()};
    m_pieces.push_back(std::move(pc));
}

void PieceTable::insert(const std::string &txt, const size_t start)
{
    if (txt.empty())
    {
        return;
    }

    m_added += txt;

    Piece new_piece = {Added, start, txt.length()};

    /* find a piece that contains the insertion location
     * then split the piece
     */
    for (auto &piece : m_pieces)
    {
        if (piece.m_length >= start)
        {

            Piece left();

            Piece right();
        }
    }
}
