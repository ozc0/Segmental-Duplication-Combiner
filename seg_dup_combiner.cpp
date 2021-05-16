#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

#define ft first
#define sc second
#define pii pair<int,int>
#define vs vector <string>
#define pss pair<segment,segment>
#define mp make_pair
#define pb push_back

struct files_str {
  char *duplication_file;
  char *output_file;
} files;

struct segment {
  string chr;
  int beg, en, strand;
  segment ( string chr0, int beg0, int en0, int strand0 ) :chr(chr0), beg(beg0), en(en0), strand(strand0){}
  segment() {chr = ""; beg = en = strand = 0;}
  bool operator== ( const segment& r) const {
    if ( this->chr.compare(r.chr) == 0 && this->beg == r.beg && this->en == r.en && this->strand == r.strand )
      return true;
    return false;
  }
  bool operator<(  const segment& r ) const {
    if ( this->chr.compare(r.chr) < 0 )
      return true;
    if ( this->chr.compare(r.chr) > 0 )
      return false;
    if ( this->beg != r.beg )
      return this->beg < r.beg;
    if ( this->en != r.en )
      return this->en < r.en;
    return this->strand < r.strand;
  }
  segment& operator = (const segment& other) {
    this->chr = other.chr;
    this->beg = other.beg;
    this->en = other.en;
    this->strand = other.strand;
    return *this;
  }
  
};

struct similar {
  segment seg1, seg2;
  int after_first, len;
};

set < segment > all_segments, super_segments;
map < segment, set < segment > > group_members;
map < segment, segment > grp;
vector < pss > pairs;
vector < similar > similars;

FILE *save;
int uneven_cnt;

void print_help () {
  fprintf(stderr,"To run this program, you need duplication file. Also, the output file must be specified in the following format.\n");
  fprintf(stderr,"\t-d [duplication file]  : Duplication file file in tab format. (see example file for identation)\n");
  fprintf(stderr,"\t-o [output file]       : output file\n");
  fprintf(stderr,"\t-h                     : help option\n");
  fprintf(stderr,"\nExample usage: ./combiner -d example_duplication.tab -o example.out\n");
}

void get_file_name ( int argc , char** argv ) {

  int opt;
  bool flag_d = 0, flag_o = 0;
  
  while ( ( opt = getopt( argc, argv , "d:o:h:" ) ) != -1 ) {
    switch ( opt ) {
    case 'd':
      files.duplication_file = optarg;
      flag_d = 1;
      break;
    case 'o':
      files.output_file = optarg;
      flag_o = 1;
      break;
    case 'h':
      print_help();
      exit(0);
      break;
    default:
      fprintf(stderr, "Invalid input type\n");
      print_help();
      exit(EXIT_FAILURE);
      break;
    }
  }

  if ( !( flag_d && flag_o ) ) {
    fprintf(stderr,"Missing one of the input/output files.\n");
    print_help();
    exit(0);
  }
}

vector < pss > read_tab();

int strand_test = 0, debugint = 0;

segment current_grp ( segment cur ) {
  if ( grp[cur] == cur )
    return cur;
  return grp[cur] = current_grp ( grp[cur] );
}

void find_groups () {
  for ( int i = 0 ; i < pairs.size() ; i++ ) {
    if ( current_grp ( pairs[i].first ) == current_grp ( pairs[i].second ) )
      continue;
    grp[ grp[pairs[i].second] ] = grp[pairs[i].first];
  }
  
  for ( auto i: grp ) {
    super_segments.insert ( i.second );
    group_members[i.second].insert ( i.first );
  }
}

void print_dups () {
  for ( auto i: group_members ) {
  	set < segment > &cur_grp = i.second;
  	set < segment > :: iterator it, it2;

  	for ( it = cur_grp.begin() ; it != cur_grp.end() ; it++ ) {
  	  string tmp = "";
      tmp = it.chr+":"+to_string(it.beg)+":"+to_string(it.en);
      fprintf(save,"%s",tmp.c_str());
      it2 = it;
      it2++;
      if ( it2 == cur_grp.end() )
      	fprintf(save,"\n");
      else
      	fprintf(save,"\t"); 
  	}
  }
}

int main( int argc, char** argv ) {

  get_file_name ( argc, argv );
  
  fprintf(stderr,"#1 Reading files\n");
  pairs = read_tab();
  fprintf(stderr,"Read files\n");

  fprintf(stderr,"#2 Finding groups\n");
  find_groups();
  fprintf(stderr,"Found groups\n");

  save = fopen(files.output_file,"w");

  fprintf(stderr,"#3 Printing\n");
  print_dups();
  
  fclose( save );
  fprintf(stderr,"DONE.\n");
  return 0;
}

vector < pss > read_tab () {

  map < string , bool > usable;
  for ( int i = 1 ; i <= 22 ; i++ ) {
    string tmp = "chr"+to_string(i);
    usable[tmp] = true;
  }

  usable["chrX"] = true;
  usable["chrY"] = true;
  usable["chrM"] = true;

  vector < pss > ret;
  ifstream file(files.duplication_file);
  
  if (file.is_open()) {
    
    string line;
    while (getline(file, line)) {

      vector < string > words;
      int st,en,i = -1;

      while ( i != line.size() ) {
	string tmp;
	for ( i++ ; i != line.size() ; i++ ) {
	  if ( line[i] == '\t' || line[i] == ' ' || line[i] == '\n' ) break;
	  tmp += line[i];
	}
	words.pb ( tmp );
      }

      int strand1 = 0, strand2 = 0;
      if ( words[8].compare("+") == 0 )
	strand1 = 1;
      if ( words[8].compare("-") == 0 )
	strand1 = 2;

      if ( words[9].compare("+") == 0 )
	strand2 = 1;
      if ( words[9].compare("-") == 0 )
	strand2 = 2;
      segment tmp1 (words[0],stoi(words[1]),stoi(words[2]),strand1);
      segment tmp2 (words[3],stoi(words[4]),stoi(words[5]),strand2);

      if ( !usable[tmp1.chr] || !usable[tmp2.chr] )
	continue;

      ret.pb ( make_pair(tmp1, tmp2) );
      all_segments.insert (tmp1);
      all_segments.insert (tmp2);
      grp[tmp1] = tmp1;
      grp[tmp2] = tmp2;
    }
    
    file.close();
  }
  else {
    fprintf(stderr,"Unable to open file %s. Make sure the system has read permition on this file and run the program again.\n",files.duplication_file);
    exit(0);
  }

  return ret;
}
