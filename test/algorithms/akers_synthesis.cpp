#include <catch.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operations.hpp>
#include <kitty/print.hpp>

#include <mockturtle/algorithms/akers_synthesis.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>

using namespace mockturtle;

TEST_CASE( "Check Akers for MAJ-3", "[akers_synthesis]" )
{
  std::vector<kitty::dynamic_truth_table> xs{5, kitty::dynamic_truth_table( 3 )};

  create_majority( xs[0] );
  for ( auto i = 0u; i < unsigned( xs[0].num_bits() ); i++ )
  {
    set_bit( xs[1], i );
  }

  auto mig = akers_synthesis<mig_network>( xs[0], xs[1] );

  kitty::create_nth_var( xs[2], 0 );
  kitty::create_nth_var( xs[3], 1 );
  kitty::create_nth_var( xs[4], 2 );

  CHECK( mig.compute( mig.index_to_node( mig.size() ), xs.begin() + 2, xs.end() ) == xs[0] );
  CHECK( mig.size() == 5 );
}

TEST_CASE( "Check Akers for MAJ-5", "[akers_synthesis]" )
{
  std::vector<kitty::dynamic_truth_table> xs{7, kitty::dynamic_truth_table( 5 )};

  create_majority( xs[0] );
  for ( auto i = 0u; i < unsigned( xs[0].num_bits() ); i++ )
  {
    set_bit( xs[1], i );
  }

  auto mig = akers_synthesis<mig_network>( xs[0], xs[1] );

  kitty::create_nth_var( xs[2], 0 );
  kitty::create_nth_var( xs[3], 1 );
  kitty::create_nth_var( xs[4], 2 );
  kitty::create_nth_var( xs[5], 3 );
  kitty::create_nth_var( xs[6], 4 );

  mig.foreach_gate( [&]( auto n ) {
    std::vector<kitty::dynamic_truth_table> fanin{3, kitty::dynamic_truth_table( 5 )};
    mig.foreach_fanin( n, [&]( auto s, auto j ) {
      fanin[j] = xs[mig.node_to_index( mig.get_node( s ) ) + 1];
    } );
    xs.push_back( mig.compute( n, fanin.begin(), fanin.end() ) );
  } );
  CHECK( xs[xs.size() - 1] == xs[0] );
}

TEST_CASE( "Check Akers for random - 4 inputs", "[akers_synthesis]" )
{
  for ( auto y = 0; y < 5; y++ )
  {
    std::vector<kitty::dynamic_truth_table> xs{6, kitty::dynamic_truth_table( 4 )};
    kitty::create_nth_var( xs[2], 0 );
    kitty::create_nth_var( xs[3], 1 );
    kitty::create_nth_var( xs[4], 2 );
    kitty::create_nth_var( xs[5], 3 );

    create_from_hex_string( xs[0], "6999" );
    //create_random( xs[0] );

    for ( auto i = 0u; i < unsigned( xs[0].num_bits() ); i++ )
    {
      set_bit( xs[1], i );
    }

    auto mig = akers_synthesis<mig_network>( xs[0], xs[1] );
    if ( mig.size() > 4 )
    {
      mig.foreach_gate( [&]( auto n ) {
        std::vector<kitty::dynamic_truth_table> fanin{3, kitty::dynamic_truth_table( 4 )};
        mig.foreach_fanin( n, [&]( auto s, auto j ) {
          if ( mig.node_to_index( mig.get_node( s ) ) == 0 )
          {
            fanin[j] = ~xs[1];
          }
          else
          {
            fanin[j] = xs[mig.node_to_index( mig.get_node( s ) ) + 1];
          }
        } );
        xs.push_back( mig.compute( n, fanin.begin(), fanin.end() ) );
      } );

      mig.foreach_po( [&]( auto n ) {
        if ( mig.is_complemented( n ) )
          CHECK( ~xs[xs.size() - 1] == xs[0] );
        else
          CHECK( xs[xs.size() - 1] == xs[0] );
      } );
    }
  }
}

TEST_CASE( "Check Akers for random - 5 inputs", "[akers_synthesis]" )
{
  for ( auto y = 0; y < 5; y++ )
  {
    std::vector<kitty::dynamic_truth_table> xs{7, kitty::dynamic_truth_table( 5 )};
    kitty::create_nth_var( xs[2], 0 );
    kitty::create_nth_var( xs[3], 1 );
    kitty::create_nth_var( xs[4], 2 );
    kitty::create_nth_var( xs[5], 3 );
    kitty::create_nth_var( xs[6], 4 );

    create_random( xs[0] );

    for ( auto i = 0u; i < unsigned( xs[0].num_bits() ); i++ )
    {
      set_bit( xs[1], i );
    }

    auto mig = akers_synthesis<mig_network>( xs[0], xs[1] );
    if ( mig.size() > 6 )
    {
      mig.foreach_gate( [&]( auto n ) {
        std::vector<kitty::dynamic_truth_table> fanin{3, kitty::dynamic_truth_table( 5 )};
        mig.foreach_fanin( n, [&]( auto s, auto j ) {
          if ( mig.node_to_index( mig.get_node( s ) ) == 0 )
          {
            fanin[j] = ~xs[1];
          }
          else
          {
            fanin[j] = xs[mig.node_to_index( mig.get_node( s ) ) + 1];
          }
        } );
        xs.push_back( mig.compute( n, fanin.begin(), fanin.end() ) );
      } );

      mig.foreach_po( [&]( auto n ) {
        if ( mig.is_complemented( n ) )
          CHECK( ~xs[xs.size() - 1] == xs[0] );
        else
          CHECK( xs[xs.size() - 1] == xs[0] );
      } );
    }
  }
}

TEST_CASE( "Check Akers for random - 6 inputs", "[akers_synthesis]" )
{
  for ( auto y = 0; y < 1; y++ )
  {
    std::vector<kitty::dynamic_truth_table> xs{8, kitty::dynamic_truth_table( 6 )};
    kitty::create_nth_var( xs[2], 0 );
    kitty::create_nth_var( xs[3], 1 );
    kitty::create_nth_var( xs[4], 2 );
    kitty::create_nth_var( xs[5], 3 );
    kitty::create_nth_var( xs[6], 4 );
    kitty::create_nth_var( xs[7], 5 );

    create_random( xs[0] );

    for ( auto i = 0u; i < unsigned( xs[0].num_bits() ); i++ )
    {
      set_bit( xs[1], i );
    }

    auto mig = akers_synthesis<mig_network>( xs[0], xs[1] );
    if ( mig.size() > 6 )
    {
      mig.foreach_gate( [&]( auto n ) {
        std::vector<kitty::dynamic_truth_table> fanin{3, kitty::dynamic_truth_table( 6 )};
        mig.foreach_fanin( n, [&]( auto s, auto j ) {
          if ( mig.node_to_index( mig.get_node( s ) ) == 0 )
          {
            fanin[j] = ~xs[1];
          }
          else
          {
            fanin[j] = xs[mig.node_to_index( mig.get_node( s ) ) + 1];
          }
        } );
        xs.push_back( mig.compute( n, fanin.begin(), fanin.end() ) );
      } );
      mig.foreach_po( [&]( auto n ) {
        if ( mig.is_complemented( n ) )
          CHECK( ~xs[xs.size() - 1] == xs[0] );
        else
          CHECK( xs[xs.size() - 1] == xs[0] );
      } );
    }
  }
}

TEST_CASE( "Check leaves iterator -- easy case ", "[akers_synthesis]" )
{
  mig_network mig;
  auto a = mig.create_pi();
  auto b = mig.create_pi();
  auto c = mig.create_pi();
  auto d = mig.create_pi();

  std::vector<mig_network::signal> operations;
  operations.push_back( mig.create_and( a, b ) );
  operations.push_back( mig.create_and( c, d ) );

  std::vector<kitty::dynamic_truth_table> xs_in{2, kitty::dynamic_truth_table( 2 )};
  std::vector<kitty::dynamic_truth_table> xs{5, kitty::dynamic_truth_table( 4 )};
  create_from_binary_string( xs_in[0], "0110" );
  for ( auto i = 0u; i < unsigned( xs_in[0].num_bits() ); i++ )
  {
    set_bit( xs_in[1], i );
  }
  auto t = akers_synthesis( mig, xs_in[0], xs_in[1], operations.begin(), operations.end() );
  mig.create_po( t );

  kitty::create_nth_var( xs[1], 0 );
  kitty::create_nth_var( xs[2], 1 );
  kitty::create_nth_var( xs[3], 2 );
  kitty::create_nth_var( xs[4], 3 );

  for ( auto i = 0u; i < unsigned( xs[1].num_bits() ); i++ )
  {
    set_bit( xs[0], i );
  }

  CHECK( mig.num_gates() == 5 );

  if ( mig.size() > 6 )
  {
    mig.foreach_gate( [&]( auto n ) {
      std::vector<kitty::dynamic_truth_table> fanin{3, kitty::dynamic_truth_table( 4 )};
      mig.foreach_fanin( n, [&]( auto s, auto j ) {
        if ( mig.node_to_index( mig.get_node( s ) ) == 0 )
        {
          fanin[j] = ~xs[0];
        }
        else
        {
          fanin[j] = xs[mig.get_node( s )];
        }
      } );
      xs.push_back( mig.compute( n, fanin.begin(), fanin.end() ) );
    } );
    mig.foreach_po( [&]( auto n ) {
      if ( mig.is_complemented( n ) )
        CHECK( ~xs[xs.size() - 1] == binary_xor( binary_and( xs[1], xs[2] ), binary_and( xs[4], xs[3] ) ) );
      else
        CHECK( xs[xs.size() - 1] == binary_xor( binary_and( xs[1], xs[2] ), binary_and( xs[4], xs[3] ) ) );
    } );
  }
}

TEST_CASE( "From klut to mig -- easy case ", "[akers_synthesis]" )
{
  klut_network klut;
  auto a = klut.create_pi();
  auto b = klut.create_pi();
  auto c = klut.create_pi();

  kitty::dynamic_truth_table tt_xor( 3u );

  kitty::create_from_hex_string( tt_xor, "69" );

  const auto n1 = klut.create_node( {a, b, c}, tt_xor );
  const auto n2 = klut.create_node( {a, c, n1}, tt_xor );
  const auto n3 = klut.create_node( {b, n1, n2}, tt_xor );
  klut.create_po( n3 );
 
  auto mig = akers_mapping( klut );

  CHECK( mig.num_pos() == 1 );
  CHECK( mig.num_pis() == 3 );
  CHECK( mig.num_gates() == 9 );

  std::vector<kitty::dynamic_truth_table> xs{4, kitty::dynamic_truth_table( 3 )};
  kitty::create_nth_var( xs[1], 0 );
  kitty::create_nth_var( xs[2], 1 );
  kitty::create_nth_var( xs[3], 2 );

  for ( auto i = 0u; i < unsigned( xs[1].num_bits() ); i++ )
  {
    set_bit( xs[0], i );
  }
  mig.foreach_gate( [&]( auto n ) {
    std::vector<kitty::dynamic_truth_table> fanin{3, kitty::dynamic_truth_table( 3 )};
    mig.foreach_fanin( n, [&]( auto s, auto j ) { 
      if ( mig.node_to_index( mig.get_node( s ) ) == 0 )
      {
        fanin[j] = ~xs[0];
      }
      else
      {
        fanin[j] = xs[mig.get_node( s )];
      }
    } );
    xs.push_back( mig.compute( n, fanin.begin(), fanin.end() ) );
  } );
  mig.foreach_po( [&]( auto n ) {
    auto func = xs[1] ^ xs[2] ^ xs[3]; 
    auto f1 = xs[1] ^ xs[3] ^ func; 
    auto f2 = xs[2] ^ func ^ f1; 
    if ( mig.is_complemented( n) )
      CHECK( ~xs[xs.size() - 1] == f2 );
    else
      CHECK( xs[xs.size() - 1] == f2 );
  } );
}
/*
TEST_CASE( "From klut to mig -- multioutput ", "[akers_synthesis]" )
{
  klut_network klut;
  auto a = klut.create_pi();
  auto b = klut.create_pi();
  auto c = klut.create_pi();
  auto d = klut.create_pi();

  kitty::dynamic_truth_table tt_xor( 4u );
  kitty::dynamic_truth_table tt_and( 4u );
  kitty::dynamic_truth_table tt_or( 4u );

  kitty::create_from_hex_string( tt_xor, "6999" );
  kitty::create_from_binary_string( tt_and, "0000000000000001" );
  kitty::create_from_binary_string( tt_or, "0111111111111111" );

  const auto n1 = klut.create_node( {a, b, c, d}, tt_xor );
  const auto n2 = klut.create_node( {a, c, d, n1}, tt_or );
  const auto n3 = klut.create_node( {b, c, n1, n2}, tt_and );
  const auto n4 = klut.create_node( {a, c, n1, n3}, tt_xor );
  klut.create_po( n3 );
  klut.create_po( n4 );
 
  auto mig = akers_mapping( klut );

  CHECK( mig.num_pos() == 2 );
  CHECK( mig.num_pis() == 4 );
  CHECK( mig.num_gates() == 14 );

  std::vector<kitty::dynamic_truth_table> xs{5, kitty::dynamic_truth_table( 4 )};
  kitty::create_nth_var( xs[1], 0 );
  kitty::create_nth_var( xs[2], 1 );
  kitty::create_nth_var( xs[3], 2 );
  kitty::create_nth_var( xs[4], 3 );

  for ( auto i = 0u; i < unsigned( xs[1].num_bits() ); i++ )
  {
    set_bit( xs[0], i );
  }
  mig.foreach_gate( [&]( auto n ) {
    std::vector<kitty::dynamic_truth_table> fanin{3, kitty::dynamic_truth_table( 4 )};
    mig.foreach_fanin( n, [&]( auto s, auto j ) { 
      std::cout << " nodo = " << n << " figlio " << mig.get_node(s) << " " << mig.is_complemented(s) << std::endl; 
      if ( mig.node_to_index( mig.get_node( s ) ) == 0 )
      {
        fanin[j] = ~xs[0];
      }
      else
      {
        fanin[j] = xs[mig.get_node( s )];
      }
    } );
    xs.push_back( mig.compute( n, fanin.begin(), fanin.end() ) );
    print_binary(xs[xs.size()-1], std::cout); std::cout << std::endl; 
  } );
  mig.foreach_po( [&]( auto n ) {
    auto func = xs[1] ^ xs[2] ^ xs[3] ^ xs[4]; 
    print_binary(func, std::cout) ; std::cout << std::endl; 
    auto f1 = binary_or(binary_or(binary_or(xs[1], xs[3]),xs[4]),func); 
    print_binary(f1, std::cout) ; std::cout << std::endl; 
    auto f2 = binary_and(binary_and(binary_and(xs[2], xs[3]), func) ,  f1); 
    print_binary(f2, std::cout) ; std::cout << std::endl; 
    auto f3 = xs[1] ^ xs[3] ^ func ^ f2; 
    std::cout << mig.get_node(n) << std::endl; 
    if ( mig.is_complemented( n ) )
    {
      if (mig.get_node(n) == 18)
      CHECK( ~xs[xs.size() - 1] == f3 );
      else 
      CHECK(~xs[xs.size() - 5] == f2);
    }
    else
    {
      if (mig.get_node(n) == 18)
      CHECK( xs[xs.size() - 1] == f3 );
      else 
      CHECK(xs[xs.size() - 5] == f2 );
    }
  } );
}*/