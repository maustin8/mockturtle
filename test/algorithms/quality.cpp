#include <catch.hpp>

#include <vector>

#include <mockturtle/algorithms/collapse_mapped.hpp>
#include <mockturtle/algorithms/cut_enumeration.hpp>
#include <mockturtle/algorithms/lut_mapping.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/io/aiger_reader.hpp>
#include <mockturtle/io/write_bench.hpp>
#include <mockturtle/views/mapping_view.hpp>

#include <fmt/format.h>
#include <lorina/aiger.hpp>

using namespace mockturtle;

template<class Ntk, class Fn, class Ret = std::result_of_t<Fn(Ntk, int)>>
std::vector<Ret> foreach_benchmark( Fn&& fn )
{
  std::vector<Ret> v;
  for ( auto const& id : {17, 432, 499, 880, 1355, 1908, 2670, 3540, 5315, 6288, 7552} )
  {
    Ntk ntk;
    lorina::read_aiger( fmt::format( "{}/c{}.aig", BENCHMARKS_PATH, id ), aiger_reader( ntk ) );
    v.emplace_back( fn( ntk, id ) );
  }
  return v;
}

TEST_CASE( "Test quality of cut_enumeration", "[quality]" )
{
  const auto v = foreach_benchmark<aig_network>( []( auto const& ntk, auto ) {
    return cut_enumeration( ntk ).total_cuts();
  } );

  CHECK( v == std::vector<unsigned>{{19, 1387, 3154, 1717, 5466, 2362, 4551, 6994, 11849, 34181, 12442}});
}

TEST_CASE( "Test quality of lut_mapping", "[quality]" )
{
  const auto v = foreach_benchmark<aig_network>( []( auto const& ntk, auto ) {
    mapping_view<aig_network, true> mapped{ntk};
    lut_mapping<mapping_view<aig_network, true>, true>( mapped );
    return mapped.num_cells();
  } );

  CHECK( v == std::vector<unsigned>{{2, 50, 68, 77, 68, 71, 97, 231, 275, 453, 347}});
}
