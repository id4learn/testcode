#include <iostream>
#include <vector>
using namespace std;

#if 0
class TestAll {
 
  public:			
	vector<int> 	vect;

	print_vect();

};
#endif

#define LOWER_RANDOM_ID 0x01
#define UPPER_RANDOM_ID 0x30


void print_vector(vector<int> vect);
void add_n_nums_in_vector(vector<int> *vect , int n);
void pop_from_vector( vector<int>  *vect);


int main() {

  // initializer list
  vector<int> vector1 = {11, 22, 33, 44, 55};

  // uniform initialization
  vector<int> vector2{6, 7, 8, 9, 10};

  // method 3
  vector<int> vector3(5, 12);

  cout << "\nvector1 = ";
	print_vector(vector1);
  cout << "\nvector2 = ";
	print_vector(vector2);
  cout << "\nvector3 = ";
	print_vector(vector3);

	add_n_nums_in_vector(&vector1 , 10);
  cout << "\nvector1 = ";
	print_vector(vector1);

	pop_from_vector( &vector1);
	return 1;

}

void add_n_nums_in_vector(vector<int> *vect , int n)
{
		int value = 0;
		int len = 0;
		vector<int>::iterator iter1;

		for( int  i = 0 ; i < n ; i++) {
				value = (rand()% (UPPER_RANDOM_ID - LOWER_RANDOM_ID +1)) + LOWER_RANDOM_ID;
				vect->push_back(value );
		}
		cout << " vector post push_back " << endl;
		print_vector(*vect);

		iter1 = vect->begin();

		for( int  i = 0 ; i < n-1 ; i++) {
				value = (rand()% (UPPER_RANDOM_ID - LOWER_RANDOM_ID +1)) + LOWER_RANDOM_ID;

				//iter1 = vect->begin();
				vect->insert(iter1,value );
				//iter1++;
		}

		cout << " vector post insert operation " << endl;
		print_vector(*vect);

	  len = vect->size();

#if 0
		for(int  i = 0; i < len; i ++) {
			iter1 = vect->begin();
		  vect->erase(iter1);
		}
#endif

		cout << " vector post erase half of elements" << endl;
		print_vector(*vect);


		return;
}

void pop_from_vector( vector<int> *vect)
{
	 int n = 0 ;
	 int value = 0;

	 n = vect->size();

	 cout << " current size of vector :" << n;
	 cout << endl;
	 cout << " poped value :" ;

	 for( int i = 0 ; i < n ; i++ ) {
			value = vect->back();
			vect->pop_back();
			cout << value << " ";
	 }
	 cout << endl;

	 //cout << " print vector "; 
	 print_vector( *vect);
	 //cout << endl;

	 return;

}


void print_vector(vector<int> vect)
{
  // declare iterator
  vector<int>::iterator iter;
	int j = 0, len = 0;

	if( vect.empty() == true) {
		cout << " Vector is empty" << endl;
		return;
	}

	len = vect.size();

#if 0
	 cout << " vector content :";
	for ( j = 0 ; j < len ; j++) {
					cout << vect.at(j) << " ";
	}
	cout << endl;
#endif

/*
  // ranged loop
  for (const int& i : vect) {
    cout << i << "  ";
	}
  cout << endl;
	*/


  // use iterator with for loop
  for (iter = vect.begin(); iter != vect.end(); iter++) {
    cout << *iter << "  ";
  }
  cout << endl;





	return;
}
