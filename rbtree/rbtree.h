/* http://algoogle.hadrori.jp/algorithm/rbtree.html
 * 修正ポイント
 * (1) nil の再定義
 *     ・nil は特定の値を持たない
 *     ・常に黒を指す
 * (2) 削除直後の修正は子供がいるかいないかの大きく２通り
 * (3) 削除後の全般修正は削除したノードの親を中心に実施
 * (4) print関数を削除して、外部にもっと凝ったものを実装する
 */ 
#include "bsearch_tree.h"

using namespace std;

template<class T> 
class rbtree: public bsearch_tree<T> {
public:
    struct rb_node;
    // テンプレートの親クラスのメンバ変数を可視化
    using bsearch_tree<T>::nil;
    using bsearch_tree<T>::root;
    using bsearch_tree<T>::sz;

    // デフォルトコンストラクタ
    rbtree();
    // デストラクタ
    ~rbtree() {}

    // 親の全ての関数を可視化
    using bsearch_tree<T>::erase;

    // 木に新しい要素を追加する。すでにキーが登録されている場合追加できない
    virtual pair<bool, typename bsearch_tree<T>::node *> insert(T key);

    // 指定ノードを削除する
    void erase(rb_node *z);

private:
    // xを中心にd方向に回転
    void rotate(typename bsearch_tree<T>::node *x, int d); 

    // 追加後の木を修正する
    void insert_update(rb_node *z);

    // 削除後の木を修正する。d によりその子x の位置を指定
    void erase_update(rb_node *y, int d);
};

/* RBノードの定義
 */

template<class T>
struct rbtree<T>::rb_node : public bsearch_tree<T>::node
{
    rb_node() {}
    rb_node(T key): bsearch_tree<T>::node(key),red(1) {}
    // 自分が親から見てどちら側の子か取得(0=左、1=右)
    int red;
    /*
    using bsearch_tree<T>::node::key;
    using bsearch_tree<T>::node::par;
    using bsearch_tree<T>::node::ch;
    */
};

/* コンストラクタ
   デフォルトコンストラクタ
 */
template<class T>
rbtree<T>::rbtree():
    bsearch_tree<T>()
{
    rb_node *p = new rb_node();
    p->red = 0;

    delete nil;
    nil = p;
    root = nil->par = nil->ch[0] = nil->ch[1] = nil;    
}

/* xを中心にd方向に回転
 * コメントはわかりやすいようにd=0の場合に統一
 */
template<class T>
void rbtree<T>::rotate(typename bsearch_tree<T>::node *x, int d) {
    int e = d^1;
    typename bsearch_tree<T>::node *y = x->ch[e];
    typename bsearch_tree<T>::node *t = y->ch[d];
    // yの左の子をxの右の子にする
    x->ch[e] = t;
    if (t != nil) t->par = x;
    // yをxの親の子にする
    y->par = x->par;
    if(y->par == nil) root = y;
    else x->par->ch[x->getChildPos()] = y;
    // xをyの左の子にする
    y->ch[d] = x;
    x->par = y;
}

/* 木に新しい要素を追加する。すでにキーが登録されている場合追加できない
 */
template<class T>
std::pair<bool, typename bsearch_tree<T>::node *> rbtree<T>::insert(T key)
{
    typename bsearch_tree<T>::node *x = root, *y = nil;  
    // キーでノードを検索         
    while(x != nil) {             
        if(key == x->key)         
            // すでにキーが登録していたら、そのノードを返す
            return make_pair(false, x); 
        y = x;                    
        x = (key < x->key)? x->ch[0]: x->ch[1];
    }                             
                                  
    // 見つからないので新規作成
    sz++;                         
    rb_node *z = new rb_node(key);   
    if (sz == 1) {                
        z->par = z->ch[0]= z->ch[1] = nil;
        root = z;                 
    }                             
    else {                        
        ((key < y->key)? y->ch[0]: y->ch[1]) = z;
        z->par = y;               
        z->ch[0] = z->ch[1] = nil;
    }                             
    insert_update(z);
    return make_pair(true, z);            
}

/* 追加後の木を修正する
 */
template<class T>
void rbtree<T>::insert_update(rb_node *z)
{
    rb_node *y;
    // 親yが赤の間繰り返す
    while((y = static_cast<rb_node*>(z->par))->red) {
        int d = y->getChildPos(), e = d^1;
        // xはzのおじ
        rb_node *x = static_cast<rb_node*>(y->par->ch[e]);
        if(x->red) {
            // ①おじが赤の場合
            y->red = 0;
            x->red = 0;
            static_cast<rb_node*>(y->par)->red = 1;
            z = static_cast<rb_node*>(y->par);
        }
        else {
            // ②おじが黒の場合
            if(z == y->ch[e]) {
                // ②→③
                z = y;
                rotate(z,d);
            }
            else {
                // ③
                y->red = 0;
                static_cast<rb_node*>(y->par)->red = 1;
                rotate(y->par,e);
            }
        }
    }
    static_cast<rb_node*>(root)->red = 0;
}


/* 指定ノードを削除する
 */
template<class T>
void rbtree<T>::erase(rb_node *z)
{
    if(z == nil) return;
    sz--;
    rb_node *w;                // 修正するノードの親（修正する部分木の根）ノード
    int d;                  // 修正するノードは左右どちらの子か
    int ex_col;             // 除去するノードの色を記憶
    if (z->ch[0] == nil || z->ch[1] == nil) {
        // 削除ノードにこどもがいないか1人だけ
        int e = (z->ch[0] == nil? 1: 0);    // こどもがいる(可能性がある)方はどっち？
        w = z->par;
        d = z->getChildPos();
        ex_col = z->red;
        replace(z, z->ch[e]);
    }
    else {
        // 削除ノードにこどもが2人
        rb_node *y;            // 部分木の最小ノードの位置
        y = minimum(z->ch[1]);
        ex_col = y->red;
        if(y->par == z) {
            // yとzが直結する場合は左斜め上に平行移動
            w = y;
            d = 1;
        }
        else {
            // yとzが離れている場合はyを木構造より除去
            w = y->par;
            d = 0;
            replace(y, y->ch[1]);
            // 削除ノードzの右の子を引き継ぐ
            y->ch[1] = z->ch[1];
            y->ch[1]->par = y;
        }
        replace(z,y);

        // 削除ノードの左の子を引き継ぐ
        y->ch[0] = z->ch[0];
        y->ch[0]->par = y;
        y->red = z->red;
    }
    
    // 黒ノードを除去した場合は修正に入る
    if(!ex_col) erase_update(w, d);
    
    delete z;
}

/* 削除後の木を修正する
 * d によりその子x の位置を指定
 */
template<class T>
void rbtree<T>::erase_update(rb_node *y, int d) 
{
    rb_node *x;
    while(y != nil && !(x = y->ch[d])->red) {
        int e = d^1;
        rb_node *w = y->ch[e];
        if(w == nil) {
            // 兄弟ノードは存在しない
            d = y->getChildPos();
            y = y->par;
        }
        if(w->red) {
            // ①兄弟ノードが赤の場合
            // ①→②～④
            w->red = 0;
            y->red = 1;
            rotate(y,d);
        }
        else {
            // ②～④兄弟ノードが黒の場合
            if(!w->ch[0]->red and !w->ch[1]->red) {
                // ②
                w->red = 1;
                d = y->getChildPos();
                y = y->par;
            }
            else if(!w->ch[e]->red) {
                // ③兄弟中心に逆回転
                // ③→④ ２段階回転
                w->red = 1;
                w->ch[d]->red = 0;
                rotate(w,e);
            }
            else {
                // ④部分木の根中心に順回転
                // ④→修正終了
                w->red = y->red;
                y->red = 0;
                w->ch[e]->red = 0;
                rotate(y,d);
                x = root;
                break;
            }
        }

    }
    x->red = 0;
}
