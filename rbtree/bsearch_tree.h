/* 
 * 赤黒木から色識別部排除
 * 再帰的でない方法 
 */ 

using namespace std;

template<class T> 
class bsearch_tree {
public:
    struct node;

    unsigned sz;
    node *root, *nil;

    // デフォルトコンストラクタ
    bsearch_tree();
    // デストラクタ
    virtual ~bsearch_tree();

    // サイズを返す
    unsigned size() const { return sz; }

    // 同一キーをもつノードを返す
    node *find(T key); 

    // 木に新しい要素を追加する。すでにキーが登録されている場合追加できない
    virtual pair<bool, node *> insert(T key); 

    // 木から要素を削除
    void erase(T key) { erase(find(key)); }

    // 指定ノードを削除する
    virtual void erase(node *z);

private:

    // 部分木uの親ノードに部分木vを接続
    void replace(node *u, node *v);
    
    // 部分木xの最小ノードを取得
    node *minimum(node* x);

};

/* ノードの定義
 */
template<class T>
struct bsearch_tree<T>::node {
    T key;
    node *par;
    node *ch[2]; // {0;left, 1;right}
    node() {}
    node(T key):key(key) {}
    // 自分が親から見てどちら側の子か取得(0=左、1=右)
    virtual ~node() {}
    int getChildPos() const { return (this == par->ch[0])? 0: 1; }
};

/* コンストラクタ
   デフォルトコンストラクタ
 */
template<class T>
bsearch_tree<T>::bsearch_tree():
    sz(0)
{
    // nilは実体を持つ
    nil = new node();
    // rootはポインタだけ
    root = nil->par = nil->ch[0] = nil->ch[1] = nil;
}

/* デストラクタ
    〜後始末〜
 */
template<class T>
bsearch_tree<T>::~bsearch_tree()
{
    while (root != nil) {
        erase(root);
    }
    delete nil;
}

/* 同一キーをもつノードを返す
 */ 
template<class T>
typename bsearch_tree<T>::node *bsearch_tree<T>::find(T key)
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
std::pair<bool, typename bsearch_tree<T>::node *> bsearch_tree<T>::insert(T key)
{
    node *x = root, *y = nil;
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
    return make_pair(true, z);
}


/* 指定ノードを削除する
 */
template<class T>
void bsearch_tree<T>::erase(node *z)
{
    if(z == nil) return;
    sz--;
    node *w;                // 修正するノードの親（修正する部分木の根）ノード
    if (z->ch[0] == nil || z->ch[1] == nil) {
        // 削除ノードにこどもがいないか1人だけ
        int e = (z->ch[0] == nil? 1: 0);    // こどもがいる(可能性がある)方はどっち？
        replace(z, z->ch[e]);
    }
    else {
        // 削除ノードにこどもが2人
        node *y;            // 部分木の最小ノードの位置
        y = minimum(z->ch[1]);
        if(y->par == z) {
            // yとzが直結する場合はなにもしない
        }
        else {
            // yとzが離れている場合はyを木構造より除去
            w = y->par;
            replace(y, y->ch[1]);
            // 削除ノードzの右の子を引き継ぐ
            y->ch[1] = z->ch[1];
            y->ch[1]->par = y;
        }
        replace(z,y);

        // 削除ノードの左の子を引き継ぐ
        y->ch[0] = z->ch[0];
        y->ch[0]->par = y;
    }
    
    delete z;
}

/* 部分木uの親ノードに部分木vを付け替え
 */
template<class T>
void bsearch_tree<T>::replace(node *u, node *v) 
{
    node *y = u->par;
    if (y == nil) root = v;
    else y->ch[u->getChildPos()] = v;
    if (v != nil) v->par = y;
}
    
/* 部分木xの最小ノードを取得
 */
template<class T>
typename bsearch_tree<T>::node *bsearch_tree<T>::minimum(node* x) 
{
    while(x->ch[0] != nil) x = x->ch[0];
    return x;
}

