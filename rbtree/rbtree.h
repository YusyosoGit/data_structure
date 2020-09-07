/* http://algoogle.hadrori.jp/algorithm/rbtree.html
 * 修正ポイント
 * (1) nil の再定義
 *     ・nil は特定の値を持たない
 *     ・常に黒を指す
 * (2) 削除直後の修正は子供がいるかいないかの大きく２通り
 * (3) 削除後の全般修正は削除したノードの親を中心に実施
 * (4) print関数を削除して、外部にもっと凝ったものを実装する
 */ 

using namespace std;

template<class T> 
class rbtree {
public:
    struct node;

    unsigned sz;
    node *root, *nil;

    // デフォルトコンストラクタ
    rbtree();
    // デストラクタ
    ~rbtree();

    // サイズを返す
    unsigned size() const { return sz; }

    // 同一キーをもつノードを返す
    node *find(T key); 

    // 木に新しい要素を追加する。すでにキーが登録されている場合追加できない
    pair<bool, node *> insert(T key); 

    // 木から要素を削除
    void erase(T key) { erase(find(key)); }

    // 指定ノードを削除する
    void erase(node *z);

private:
    // xを中心にd方向に回転
    void rotate(node *x, int d); 

    // 追加後の木を修正する
    void insert_update(node *z);

    // 部分木uの親ノードに部分木vを接続
    void replace(node *u, node *v);
    
    // 部分木xの最小ノードを取得
    node *minimum(node* x);

    // 削除後の木を修正する。d によりその子x の位置を指定
    void erase_update(node *y, int d);
};

/* ノードの定義
 */
template<class T>
struct rbtree<T>::node {
    T key;
    int red;
    node *par, *ch[2]; // {left, right}
    node() {}
    node(T key):key(key),red(1) {
        //par = ch[0] = ch[1] = NULL;
    }
    // 自分が親から見てどちら側の子か取得(0=左、1=右)
    int getChildPos() const { return (this == par->ch[0])? 0: 1; }
};

/* コンストラクタ
   デフォルトコンストラクタ
 */
template<class T>
rbtree<T>::rbtree():
    sz(0)
{
    nil = new node();
    nil->red = 0;
    root = nil->par = nil->ch[0] = nil->ch[1] = nil;
}

/* デストラクタ
    〜後始末〜
 */
template<class T>
rbtree<T>::~rbtree()
{
    while (root != nil) {
        erase(root);
    }
    delete nil;
}

/* xを中心にd方向に回転
 * コメントはわかりやすいようにd=0の場合に統一
 */
template<class T>
void rbtree<T>::rotate(node *x, int d) {
    int e = d^1;
    node *y = x->ch[e];
    node *t = y->ch[d];
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

/* 同一キーをもつノードを返す
 */ 
template<class T>
typename rbtree<T>::node *rbtree<T>::find(T key)
{
    node *x = root;
    while(x != nil) {
        if(key == x->key) return x;
        x = (key < x->key)? x->ch[0]: x->ch[1];
    }
    return x;
}

/* 木に新しい要素を追加する。すでにキーが登録されている場合追加できない
 */
template<class T>
std::pair<bool, typename rbtree<T>::node *> rbtree<T>::insert(T key)
{
    node *x = root, *y = nil;
    int d;
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
    node *z = new node(key);
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
void rbtree<T>::insert_update(node *z)
{
    node *y;
    // 親yが赤の間繰り返す
    while((y = z->par)->red) {
        int d = y->getChildPos(), e = d^1;
        // xはzのおじ
        node *x = y->par->ch[e];
        if(x->red) {
            // ①おじが赤の場合
            y->red = 0;
            x->red = 0;
            y->par->red = 1;
            z = y->par;
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
                y->par->red = 1;
                rotate(y->par,e);
            }
        }
    }
    root->red = 0;
}


/* 指定ノードを削除する
 */
template<class T>
void rbtree<T>::erase(node *z)
{
    if(z == nil) return;
    sz--;
    node *w;                // 修正するノードの親（修正する部分木の根）ノード
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
        node *y;            // 部分木の最小ノードの位置
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

/* 部分木uの親ノードに部分木vを接続
 */
template<class T>
void rbtree<T>::replace(node *u, node *v) 
{
    node *y = u->par;
    if (y == nil) root = v;
    else y->ch[u->getChildPos()] = v;
    if (v != nil) v->par = y;
}
    
/* 部分木xの最小ノードを取得
 */
template<class T>
typename rbtree<T>::node *rbtree<T>::minimum(node* x) 
{
    while(x->ch[0] != nil) x = x->ch[0];
    return x;
}

/* 削除後の木を修正する
 * d によりその子x の位置を指定
 */
template<class T>
void rbtree<T>::erase_update(node *y, int d) 
{
    node *x;
    while(y != nil && !(x = y->ch[d])->red) {
        int e = d^1;
        node *w = y->ch[e];
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
