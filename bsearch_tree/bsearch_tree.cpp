/*
 *	赤黒木（２分探索法）
 *	生成＆出力プログラム
 *	[コマンド] [ランダム生成要素数|数値リストのファイル]
 *	[コマンド] [ランダム生成要素数]　[数値リストのファイル]
 *
 *	引数は１つか２つかを選ぶことが出来る。
 *
 *	引数が１つの時は要素数かファイル名を指定。
 *		N個の要素の自動生成かファイルに記載の数値をもとに赤黒木の生成を行う
 *	引数が２つの時は引数１に要素数　引数２にファイル名を指定する。
 *		N個の要素を自動生成した後、ファイル記載の要素を足したり引いたりする
 */

#include <string.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <stdexcept>// 数値に変換できなかったとき例外を投げる
#include <iomanip> // マニピュレータ
#include "bsearch_tree.h"

using namespace std;

#define E int
//E elem[] = {0, 2, 9, 10, 12, 5, 3, 29, 20, 16};

/* 引数で受け取った赤黒木を標準出力する
 * 一行につき一個の要素を表示。
 * 深さが増すほど右寄りに表示する
 */
template<class T>
class printer
{
	typedef pair<T, T> pr_val;
	typedef vector<pr_val> bcrumb;
	class bcrumb::iterator it;// class はprinter<T>::bcrumb::iteratorの前に必要
	bcrumb bough;			// 横枝：上限と下限の値に挟まれた範囲
	const bsearch_tree<T> &tree;
public:
	printer(const bsearch_tree<T> &rbt);
	void out(const struct bsearch_tree<T>::node *v, int dep, int lr);
	void out();
};

template<class T>
printer<T>::printer(const bsearch_tree<T> &rbt):
	tree(rbt)
{
	cerr << hex << uppercase;
}

/* 関数：指定のノードを指定の深さに表示
 * 引数１：ノード
 * 引数２：深さ
 * 引数３：親ノードとの関係 0:親なし（根） 1:長子(左) 2:次子(右) 
 */
template<class T>
void printer<T>::out(const struct bsearch_tree<T>::node *v, int dep, int lr)
{
	if (v == tree.nil) {
		return;
	}
	// 横枝の範囲登録
	pr_val pr = make_pair(v->key, v->key);
	if (v->ch[0] != tree.nil) pr.first = v->ch[0]->key;
	if (v->ch[1] != tree.nil) pr.second = v->ch[1]->key;
	bough.push_back(pr);

	// 再帰呼出し: 次子（右の子）を指定
	out(v->ch[1], dep+1, 2);

	// 自分より浅い位置にある横枝の表示
	auto it= bough.begin();
	for (int i=0; i< dep; i++, it++) {
		if (v->key < (*it).first || (*it).second < v->key) {
			// 横枝が無いもしくは横枝の範囲外
			cerr << "  ";
		}
		else {
			cerr << "│ ";
		}
	}

	// 葉または節の表示
	cerr << (!lr? "─ ": (lr == 2?  "┌ ": "└ "));
    cerr << "\x1b[32;1m"; // 緑色、太字
	cerr << right << setfill('0') << setw(2) << v->key << endl;
    cerr << "\x1b[0m";

	// 再帰呼出し：長子（左の子）を指定
	out(v->ch[0], dep+1, 1);

	// 横枝を除去
	bough.pop_back();
}

template<class T>
void printer<T>::out()
{
	if (tree.size() == 0) {
		cerr << "─ " << endl;
		return;
	}
	pr_val pr = make_pair(tree.root->key, tree.root->key);
	bough.push_back(pr);

	out(tree.root, 0, 0);
}

int main(int argc, char * argv[])
{
	// 要素ゼロのリスト生成
    bsearch_tree<E> tree;
    // 出力プログラムに通知
	printer<E> dsp(tree);

    if (argc == 1) {
		cout << "USAGE1:" << argv[0] << " <file of elements to add>" << endl;
        cout << "USAGE2:" << argv[0] << " <num of elements> [file of elements to add or remove]" << endl;

        return 0;
    }
	// 要素のあるファイル名	
	const char * entry;
	int nElems;
	bool bNumberAssigned;
	try {
		nElems = std::stoi(argv[1]);
		bNumberAssigned = true;
		cout << "generating " << nElems << " element(s)." << endl;
	}
	catch (...) {
		nElems = 0;
		bNumberAssigned = false;
	}

	switch ( argc ) 
	{
	case 2:
		// 引数一つだけ
		if (bNumberAssigned)
			// 引数が要素数／ファイル名なし
			entry = NULL;
		else
			// 要素数なし／引数がファイル名
			entry = argv[1];
		break;

	case 3:
		// 引数二つ
		if (!bNumberAssigned) { 
			// 引数１が数値ではないため異常終了
			cout << "arg1 ought to be numeric." << endl;
			return 1;
		}
		else {
			// 引数１が要素数／引数２がファイル名
			entry = argv[2];
		}
		break;

	default:
		// エラー
		cout << "bad arguments." << endl;
		return 1;
	}

	if (nElems)
	{
		// 要素数未設定の場合要素を設定
		srand(0);
		vector<int> deck;
		deck.reserve(255);
		for (int i= 0; i< 255; i++) {
			deck.push_back(i);
		}
		for (int i= 0; i< nElems; i++) {
			int index = rand() % deck.size();
			tree.insert(deck[index]);
			deck.erase(deck.begin() + index);
		}
	}

	if (entry) {
		// ファイルから要素を読み込む	
		ifstream in(entry);
/*	for (int i= 0, n= sizeof(elem)/ sizeof(E); i < n; i++)
		tree.insert(elem[i]);
*/
		if (!in) {
			cout << "file " << entry << " not found." << endl;
			return (1);
		}


        cout << hex << uppercase;
		E e;
		char str[16], *endptr;
		while (!in.eof()) {
			in >> str;
			e = strtol(str, &endptr, 16);
			// eofのみでファイルの終わりを正確に検出できないので、終了の印-1を用いる
			if (e == -1) break;
            cout << right << setfill('0') << setw(2);
            cout << e << ":";
			auto pr = tree.insert(e);
			// 同じ数値を入力するとそれを削除する
			if (!pr.first) {
				tree.erase(pr.second);
                cout << right << setfill('0') << setw(2);
				cout << e << " erased." << std::endl;
			}
			else {
                cout << right << setfill('0') << setw(2);
				cout << e << " inserted." << endl;
			}
		}
	}
	// １６進、大文字使用、右詰め、０埋め、２桁
	dsp.out();
	return 0;
}

