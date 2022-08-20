#include<iostream>
#include<unordered_map>
#include<vector>
using namespace std;

#define LOWER_RANDOM_ID 0x01
#define UPPER_RANDOM_ID 0x30




int add_entry_in_umap(unordered_map<int,int> *umap);
int display_umap(unordered_map<int,int> *umap);



int main()
{
	unordered_map<int,int>  umap;

	add_entry_in_umap(&umap);
	display_umap(&umap);

	return 1;
}

int add_entry_in_umap(unordered_map<int,int> *umap)
{
	int value = 0;
	vector<int>	vect;
	vector<int>::iterator  vitr;
	unordered_map<int,int>::iterator umap_itr;
	//auto  vitr;

	for( int i = 0; i < 10 ; i++) {
		value = (rand()% (UPPER_RANDOM_ID - LOWER_RANDOM_ID +1)) + LOWER_RANDOM_ID;
		umap->insert(make_pair(i+10, value));

		vect.push_back(i+10);
	}

	//len = vect.size();

	cout << "key" << " " << "value" << endl;
	for ( vitr = vect.begin() ; vitr != vect.end(); vitr++) {

					cout << "key:" << *vitr << " ";
					umap_itr = umap->find(*vitr) ;
					if(umap_itr != NULL ) {
									cout << umap_itr->second;
					}
					else {
								 cout << " not found" ;
					}

					cout << endl;
	}

	return 1;


}

int display_umap(unordered_map<int,int> *umap)
{
	unordered_map<int,int>::iterator   it;

	if(umap->empty() ) {
					cout << " unorder_map umap is empty" << endl;
					return 1;
	}

#if 1
	cout<< "key" << "  "<< "value" << endl  ;
	for( it = umap->begin(); it != umap->end() ; it++) {
				cout<< it->first << "  " << it->second << endl;

	}
#endif

	return 1;

}



