// inserting into a list
#include <iostream>
#include <list>
#include <vector>
using namespace std;

#define LOWER_RANDOM_ID 0x01
#define UPPER_RANDOM_ID 0x30


typedef struct data_s {
		uint32_t	id;
		uint32_t	value;
}data_t;

void add_n_element_in_list( list<data_t> *mylist, int n);
void print_all_element_in_list( list<data_t> *mylist);

int main ()
{
	list<data_t> mylist;
	list<data_t>:: iterator it_list;
	int n = 10;

	// add n elements in list
	add_n_element_in_list( &mylist,  n);


}

void add_n_element_in_list( list<data_t> *mylist, int n)
{
	data_t	elem = {0};
	list<data_t>::iterator itr_mylist;

		for( int i = 0 ; i < n ; i++) {
				elem.value = (uint32_t) ((rand()% (UPPER_RANDOM_ID - LOWER_RANDOM_ID +1)) + LOWER_RANDOM_ID);
				elem.id = i;
				mylist->push_back(elem);

				// insert same element at begining
				itr_mylist = mylist->begin();
				mylist->insert(itr_mylist, elem);
		}

		print_all_element_in_list( mylist);

}

void print_all_element_in_list( list<data_t> *mylist)
{
	data_t	elem;

	for( auto itr = mylist->begin(); itr != mylist->end(); ++itr) {
			elem = *itr;
			cout << " id:" << elem.id << " value:" << elem.value << endl;
			elem = mylist->front();
			cout << " id:" << elem.id << " value:" << elem.value << endl;
	}

	return;


}



#if 0
int simple_list()
{
  std::list<int> mylist;
  std::list<int>::iterator it;

  // set some initial values:
  for (int i=1; i<=5; ++i) mylist.push_back(i); // 1 2 3 4 5

  it = mylist.begin();
  ++it;       // it points now to number 2           ^

  mylist.insert (it,10);                        // 1 10 2 3 4 5

  // "it" still points to number 2                      ^
  mylist.insert (it,2,20);                      // 1 10 20 20 2 3 4 5

  --it;       // it points now to the second 20            ^

  std::vector<int> myvector (2,30);
  mylist.insert (it,myvector.begin(),myvector.end());
                                                // 1 10 20 30 30 20 2 3 4 5
                                                //               ^
  std::cout << "mylist contains:";
  for (it=mylist.begin(); it!=mylist.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';

  return 0;
}
#endif
