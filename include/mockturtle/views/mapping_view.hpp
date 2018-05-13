/* mockturtle: C++ logic network library
 * Copyright (C) 2018  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  \file mapping_view.hpp
  \brief Implements mapping methods to create mapped networks

  \author Mathias Soeken
*/

#pragma once

#include <vector>

#include "../networks/detail/foreach.hpp"
#include "../traits.hpp"
#include "../utils/truth_table_cache.hpp"
#include "../views/immutable_view.hpp"

#include <kitty/dynamic_truth_table.hpp>

namespace mockturtle
{

namespace detail
{

template<bool StoreFunction>
struct mapping_view_storage;

template<>
struct mapping_view_storage<true>
{
  std::vector<uint32_t> mappings;
  uint32_t mapping_size{0};
  std::vector<uint32_t> functions;
  truth_table_cache<kitty::dynamic_truth_table> cache;
};

template<>
struct mapping_view_storage<false>
{
  std::vector<uint32_t> mappings;
  uint32_t mapping_size{0};
};

} // namespace detail

template<typename Ntk, bool StoreFunction = false>
class mapping_view : public immutable_view<Ntk>
{
public:
  using storage = typename Ntk::storage;
  using node = typename Ntk::node;
  using signal = typename Ntk::signal;

  /*! \brief Default constructor.
   *
   * Constructs mapping view on another network.
   */
  mapping_view( Ntk const& ntk ) : immutable_view<Ntk>( ntk )
  {
    static_assert( is_network_type_v<Ntk>, "Ntk is not a network type" );
    static_assert( has_size_v<Ntk>, "Ntk does not implement the size method" );
    static_assert( has_node_to_index_v<Ntk>, "Ntk does not implement the node_to_index method" );

    _mapping_storage.mappings.resize( ntk.size(), 0 );

    if constexpr ( StoreFunction )
    {
      /* insert 0 truth table */
      _mapping_storage.cache.insert( kitty::dynamic_truth_table( 0 ) );

      /* default each truth table to 0 */
      _mapping_storage.functions.resize( ntk.size(), 0 );
    }
  }

  bool has_mapping()
  {
    return _mapping_storage.mapping_size > 0;
  }

  bool is_mapped( node const& n ) const
  {
    return _mapping_storage.mappings[this->node_to_index( n )] != 0;
  }

  void clear_mapping()
  {
    _mapping_storage.mappings.clear();
    _mapping_storage.mappings.resize( this->size(), 0 );
    _mapping_storage.mapping_size = 0;
  }

  uint32_t num_luts() const
  {
    return _mapping_storage.mapping_size;
  }

  template<typename LeavesIterator>
  void add_to_mapping( node const& n, LeavesIterator begin, LeavesIterator end )
  {
    auto& mindex = _mapping_storage.mappings[this->node_to_index( n )];

    /* increase mapping size? */
    if ( mindex == 0 )
    {
      _mapping_storage.mapping_size++;
    }

    /* set starting index of leafs */
    mindex = _mapping_storage.mappings.size();

    /* insert number of leafs */
    _mapping_storage.mappings.push_back( std::distance( begin, end ) );

    /* insert leaf indexes */
    while ( begin != end )
    {
      _mapping_storage.mappings.push_back( this->node_to_index( *begin++ ) );
    }
  }

  void remove_from_mapping( node const& n )
  {
    auto& mindex = _mapping_storage.mappings[this->node_to_index( n )];

    if ( mindex != 0 )
    {
      _mapping_storage.mapping_size--;
    }

    _mapping_storage.mappings[this->node_to_index( n )] = 0;
  }

  template<bool enabled = StoreFunction, typename = std::enable_if_t<std::is_same_v<Ntk, Ntk> && enabled>>
  kitty::dynamic_truth_table lut_function( node const& n )
  {
    return _mapping_storage.cache[_mapping_storage.functions[this->node_to_index( n )]];
  }

  template<bool enabled = StoreFunction, typename = std::enable_if_t<std::is_same_v<Ntk, Ntk> && enabled>>
  void set_lut_function( node const& n, kitty::dynamic_truth_table const& function )
  {
    _mapping_storage.functions[this->node_to_index( n )] = _mapping_storage.cache.insert( function );
  }

  template<typename Fn>
  void foreach_lut_fanin( node const& n, Fn&& fn ) const
  {
    const auto it = _mapping_storage.mappings.begin() + _mapping_storage.mappings[this->node_to_index( n )];
    const auto size = *it++;
    foreach_element_transform( it, it + size,
                               [&]( auto i ) { return this->index_to_node( i ); }, fn );
  }

private:
  detail::mapping_view_storage<StoreFunction> _mapping_storage;
};

} // namespace mockturtle
