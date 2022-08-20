#include<iostream>
#include<string>
#include<cstring>

using namespace std;


const int ALPHABET_SIZE = 26;
  
// trie node
struct TrieNode {
    struct TrieNode *node[ALPHABET_SIZE];
    bool isEndOfWord;  // isEndOfWord is true if the node represents end of a word
};

TrieNode* allocTrieNode()
{
		TrieNode* tmp = NULL;
    tmp = ((TrieNode *)malloc(sizeof (TrieNode)));

		memset((void *) tmp, 0 , sizeof(TrieNode));
		return tmp;
}


class Trie {
    TrieNode    *head;
public:
    Trie() {
       //head = (TrieNode *)malloc(sizeof (TrieNode));
       head = allocTrieNode();
    }
    
    void insert(string word) {
    
        TrieNode *tmp = head;
        int len =0, i = 0;
        int index = 0;
        
        len = word.length();
        
        if(len == 0)
            return;
        
        while(i < len) {
            
            // calculate char index
            if(word[i] >= 'A' && word[i] <= 'Z')
                index = word[i++] - 'A';
            else if(word[i] >= 'a' && word[i] <= 'z')
                index = word[i++] - 'a';
            else
                return; // invalid insert
            
            if( tmp->node[index] == NULL) {
                TrieNode *newNode = allocTrieNode();
                tmp->node[index] = newNode;
            }
            tmp = tmp->node[index];
        }
        tmp->isEndOfWord = true;
    }
    
    
    
    bool search(string word) {
        TrieNode *tmp = head;
        int len =0, i = 0;
        int index = 0;
        
        len = word.length();
        
        if(len == 0)
            return false;
        
        while(i < len) {
            
            // calculate char index
            if(word[i] >= 'A' && word[i] <= 'Z')
                index = word[i++] - 'A';
            else if(word[i] >= 'a' && word[i] <= 'z')
                index = word[i++] - 'a';
            else
                return false; // invalid string search
            
            if( tmp->node[index] == NULL) {
                return false;
            }
            tmp = tmp->node[index];
        }//while
        if(tmp->isEndOfWord == false)
            return false;
        else 
            return true;
    }
    
    bool startsWith(string word) {
        TrieNode *tmp = head;
        int len =0, i = 0;
        int index = 0;
        
        len = word.length();
        
        if(len == 0)
            return false;
        
        while(i < len) {
            
            // calculate char index
            if(word[i] >= 'A' && word[i] <= 'Z')
                index = word[i++] - 'A';
            else if(word[i] >= 'a' && word[i] <= 'z')
                index = word[i++] - 'a';
            else
                return false; // invalid string search
            
            if( tmp->node[index] == NULL) {
                return false;
            }
            tmp = tmp->node[index];
        }//while
        
       return true; //prefix match
    }
};

int main()
{
	string keys[] = {"the", "a", "there", "answer", "any", "by", "bye", "their" , "apple"  };
	int n = sizeof(keys)/sizeof(keys[0]);


//	["Trie","insert","search","search","startsWith","insert","search"]
//[[],["apple"],["apple"],["app"],["app"],["app"],["app"]]

	//struct TrieNode *root = getNode();
	
	Trie	dict;

	// Construct trie
	for (int i = 0; i < n; i++)
		dict.insert( keys[i]);


	dict.search("the")? cout << "Yes\n" : cout << "No\n";
	dict.search("these")? cout << "Yes\n" : cout << "No\n";
	dict.search("their")? cout << "Yes\n" : cout << "No\n";
  dict.search("thaw")? cout << "Yes\n" : cout << "No\n";

  //dict.startsWith("app")? cout << "Yes\n" : cout << "No\n";
  dict.search("app")? cout << "Yes\n" : cout << "No\n";
	dict.insert("app");
  //dict.startsWith("app")? cout << "Yes\n" : cout << "No\n";
  dict.search("app")? cout << "Yes\n" : cout << "No\n";
  //dict.search("app")? cout << "Yes\n" : cout << "No\n";

	return 1;

}
