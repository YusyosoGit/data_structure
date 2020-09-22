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
#include <typeinfo>
#include <iostream>
#include <stdexcept>// 数値に変換できなかったとき例外を投げる
#include <iomanip> // マニピュレータ
#include "rbtree.h"

using namespace std;

// コンパイルオプション
#define E int

enum TREE_TYPE {
   BSEARCH = 0,
   RED_BLACK,
};

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
    void out(const struct bsearch_tree<T>::node *v);
	void out(const struct bsearch_tree<T>::node *v, int dep, int lr);
	void out();
};

template<class T>
printer<T>::printer(const bsearch_tree<T> &rbt):
	tree(rbt)
{
	cerr << hex << uppercase;
}

/*
 * ノードを表示する
*/
template<class T>
void printer<T>::out(const struct bsearch_tree<T>::node *v)
{
    if (typeid(*v) == typeid(const struct rbtree<T>::rb_node)) {
        const struct rbtree<T>::rb_node* p= static_cast<const struct rbtree<T>::rb_node*>(v);
        cerr << (p->red? "\x1b[31;1m": "\x1b[30;1m");
    }
    else {
        cerr << "\x1b[32;1m";// "1"はBoldのオプション
    }
    cerr << right << setfill('0') << setw(2) << v->key << endl;
    cerr << "\x1b[0m";
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
	out(v); 

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

/*
 * エントリーポイント
 */ 
int main(int argc, char * argv[])
{
    if (argc == 1) {
        cout << "USAGE1:" << argv[0] << " <ファイル>" << endl;
        cout << "USAGE2:" << argv[0] << " <要素数> [ファイル]" << endl;
        cout << "Options:" << endl;
        cout << "-o, --ordinary         通常の２分木" << endl;
        cout << "-r, --red_and_black    赤黒木（デフォルト)" << endl;
        cout << "-e, --elements         要素数：自動生成する要素の数" << endl;
        cout << "-f, --file             ファイル：追加・削除の要素が記されたファイル" << endl;
        cout << "-s, --seed             乱数の種（要素自動生成時に使用）" << endl;
        cout << endl;
        return 0;
    }

    const char * entry;
    int nElems = 0;
    int seed = 0;
    int tree_type= RED_BLACK;

    for (int i=1; i< argc; ) {
        string opt(argv[i]);
        try {

            if (opt == "-o" || opt == "--ordinary") {
                tree_type = BSEARCH;
            }
            else if (opt == "-r" || opt == "--red_and_black") {
                tree_type = RED_BLACK;
            }
            else if (opt == "-e" || opt == "--elements") {
                if (++i>= argc) throw 0;
                nElems = std::stoi(argv[i]);
            }
            else if (opt =="-f" || opt == "--file") {
                if (++i>= argc) throw 0;
                entry = argv[i];
            }
            else if (opt =="-s" || opt == "--seed") {
                if (++i>= argc) throw 0;
                seed = std::stoi(argv[i]);
            }
            else {
                throw opt;
            }
        }
        catch (std::invalid_argument) {
            // 非数値
            cout << argv[i] << " is not numeric." << endl;
            return 1;
        }
        catch (int) {
            // 引数が少ない
            cout << "too few arguments." << endl;
            return 1;
        }
        catch (string &opt) {
            // 引数エラー
            cout << "bad argument " << "\"" << opt << "\"" << endl;
            return 1;
        }
        catch (...) {
            // その他のエラー
            cout << "impossible." << endl;
            return 1;
        }
        i++;
    }

    // 要素ゼロのリスト生成
    bsearch_tree<E> *tree;
    if (tree_type == BSEARCH) {
        tree = new bsearch_tree<E>;
    }
    else {
        tree = new rbtree<E>;
    }
    // 出力プログラムに通知
    printer<E> dsp(*tree);

    if (nElems)
    {
        cout << "generating " << nElems << " element(s)." << endl;
        // 要素数未設定の場合要素を設定
        srand(seed);
        vector<int> deck;
        deck.reserve(255);
        for (int i= 0; i< 255; i++) {
            deck.push_back(i);
        }
        for (int i= 0; i< nElems; i++) {
            int index = rand() % deck.size();
            tree->insert(deck[index]);
            deck.erase(deck.begin() + index);
        }
    }

    if (entry) {
        // ファイルから要素を読み込む
        ifstream in(entry);
        if (!in) {
            cout << "file " << entry << " not found." << endl;
            return (1);
        }

        cout << hex << uppercase;
        E e;
        char str[16], *endptr;
        in >> str;
        while (!in.eof()) {
            e = strtol(str, &endptr, 16);
            // eofのみでファイルの終わりを正確に検出できないので、終了の印-1を用いる
            //if (e == -1) break;
            cout << right << setfill('0') << setw(2);
            cout << e << ":";
            auto pr = tree->insert(e);
            if (!pr.first) {
                // 既存の値は削除する
                tree->erase(pr.second);
                cout << right << setfill('0') << setw(2);
                cout << e << " erased." << std::endl;
            }
            else {
                // 新規の値は追加する
                cout << right << setfill('0') << setw(2);
                cout << e << " inserted." << endl;
            }
            in >> str;
        }
    }
    // １６進、大文字使用、右詰め、０埋め、２桁
    dsp.out();
    return 0;
}

