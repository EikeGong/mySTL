typedef int Rank; //秩
#define DEFAULT_CAPACITY 3  //默认初始容量（实际应用中可以设置更大）
#include "Fib.cpp"

template<typename T> class Vector { //向量模版类
protected:
    Rank _size; //规模
    int _capacity;  //容量
    T *_elem;   //向量所在，指针，指向数据区首地址
    void copyFrom(T const *A, Rank lo, Rank hi); //复制
    void expand(); //空间不足时扩容
    void shrink(); //装填因子过小时缩容
    Rank bubble(Rank lo, Rank hi);  //扫描交换
    void bubbleSort();  //起泡排序算法
    Rank max(Rank lo, Rankhi);  //选取最大元素
    void selectionSort(Rank lo, Rank hi);   //选择排序
    void merge(Rank lo, Rank mi, Rank hi);  //归并算法
    void mergeSort(Rank lo, Rank hi);   //归并排序
    void heapSort(Rank lo, Rank hi);    //堆排序
    Rank partition(Rank lo, Rank hi);   //轴点构造算法
    void quickSort(Rank lo, Rank hi);   //快速排序
    void shellSort(Rank lo, Rank hi);   //希尔排序
public:
    //构造函数
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0) {  //容量为c、规模为s、所有元素初始为v
        _elem = new T[_capacity = c];      //申请初始容量的内存空间，并返回指针
        for (_size = 0; _size < s; _elem[_size ++] = v);    //为vector中每个元素赋值，每赋值依次_size增大
    }
    Vector(T const *A, Rank n) {copyFrom(A, 0, n);}  //复制构造，作用于数组，整体复制
    Vector(T const *A, Rank lo, Rank hi) {copyFrom(A, lo, hi);}     //复制构造，作用于数组，区间复制
    Vector(Vector<T> const &V) {copyFrom(V._elem, 0, V._size);}  //复制构造，作用于向量，整体复制
    Vector(Vector<T> const &V, Rank lo, Rank hi) {copyFrom(V._elem, lo, hi);}  //复制构造，作用于向量，区间复制

    //析构函数
    ~Vector() {delete[] _elem;}

    //只读访问接口
    Rank size() const {return _size;} //返回规模
    bool empty() const {return  !_size;}    //判空，为0则为空返回1
    Rank find(T const &e) const {return find(e, 0, _size);}   //无序向量整体查找
    Rank find(T const &e, Rank lo, Rank hi) const;  //无序向量区间查找
    Rank search(T const &e) const {return search(e, 0, _size);}   //有序向量整体查找
    Rank search(T const &e, Rank lo, Rank hi) const;    //有序向量区间查找
    int disordered() const;

    //可写访问接口
    T & operator[](Rank r);  //重载下标操作符
    const T & operator[](Rank r) const; //仅限用于右值，即只读的重载版本
    Vector<T> & operator=(Vector<T> const &);//重载赋值操作符，以便于直接克隆
    T remove(Rank r);   //删除秩为r的元素，返回该元素
    int remove(Rank lo, Rank hi);   //删除秩在区间[lo, hi)之内的元素，返回删除元素的个数
    Rank insert(Rank r, T const &e);    //插入元素
    Rank insert(T const &e);    //默认插入到最后
    void sort(Rank lo, Rank hi);    //对[lo, hi)排序
    void sort() {sort(0, _size);}   //对整体排序
    void unsort(Rank lo, Rank hi);  //对[lo, hi)置乱
    void unsort() {unsort(0, _size);}  //对[lo, hi)排序
    int deduplicate();  //无序去重
    int uniquify(); //有序去重
    void traverse(void (*)(T &));    //遍历（使用函数指针，只读或局部修改）
    template<typename VST> void traverse(VST &); //遍历（使用函数对象，全局修改）
};

template<typename T>
void Vector<T>::copyFrom(const T *A, Rank lo, Rank hi) {//深度复制
    _elem = new T[_capacity = (hi - lo) << 1];   //分配空间
    _size = 0;  //将规模置0
    while(lo < hi) {
        _elem[_size ++] = A[lo ++];     //将原数组的元素依次复制到新空间
    }
}

template<typename T>
void Vector<T>::expand() {
    if(_size < _capacity) return; //未满员时不必扩容
    if(_capacity < DEFAULT_CAPACITY) _capacity = DEFAULT_CAPACITY;  //不得低于最小容量
    T *oldElem = _elem;     //原空间分配给另一个指针，该指针在本方法执行结束后会被丢弃
    _elem = new T[(_capacity << 1 ) + 1];  //新空间容量加倍
    for (int i = 0; i < _size; i++) {
        _elem[i] = oldElem[i];
    }
    delete[] oldElem;
}

template<typename T>
T &Vector<T>::operator[](Rank r) {  //默认r合法，实际应用上应该加严格判断
    return _elem[r];
}

template<typename T>
const T &Vector<T>::operator[](Rank r) const {  //仅限于做右值
    return _elem[r];
}


template<typename T>
Vector<T> &Vector<T>::operator=(const Vector<T> &v) {
    return Vector(v);
}

template<typename T>
Rank Vector<T>::insert(Rank r, const T &e) {    //默认r合法，实际应用上应该加严格判断
    expand();   //先扩容
    for(int i = _size; i > r; i--) {    //自后往前
        _elem[i] = _elem[i - 1];    //每个元素后移
    }
    _elem[r] = e;   //置入新元素
    _size ++;
    return r;   //返回秩，当然也可以返回插入是否成功
}

template<typename T>
Rank Vector<T>::insert(const T &e) {    //默认插到最后一位
    expand();
    _elem[_size] = e;
    _size ++;
    return _size - 1;
}

template<typename T>
int Vector<T>::remove(Rank lo, Rank hi) {
    if (lo >= hi) return 0; //出于效率考虑，单独处理特殊情况
    while (hi < _size) _elem[lo ++] = _elem[hi ++];
    _size = lo; //这里实际上通常_size后面还是存储着数据但是将他们视为垃圾数据
    return hi - lo; //返回删除的元素数目
}

template<typename T>
T Vector<T>::remove(Rank r) {   //删除单个元素，将其视为区间元素的特殊形式[r,r-1)
    T e = _elem[r];
    remove(r, r + 1);
    return e;
}

template<typename T>
Rank Vector<T>::find(const T &e, Rank lo, Rank hi) const {  //一般根据不同的类型要重新设定判等器
    while ((lo < hi --) && (e != _elem[hi]));   //逆向查找，返回的是查找到的秩最大者，如果是正向查找则返回秩最小者
    return hi;  //返回的值比传入的lo小即查找失败，否则返回的是命中元素的秩
}

template<typename T>
int Vector<T>::deduplicate() {
    int oldSize = _size;    //记录原本数目
    Rank i = 1;
    while (i < _size) { //遍历每一个元素
        find(_elem[i], 0, i) < 0 ? i++ :remove(i);  //对每个元素的前序进行查找，如有重复，则直接将i丢弃，丢弃后新的元素仍然是i，如没有则i++
    }   //看似 _size 没有改变，但其实remove中不断在改变 _size 的值
    return oldSize - _size; //返回删除的元素数
}

template<typename T>
void Vector<T>::traverse(void (*visit)(T &)) {
    for (int i = 0; i < _size; i++) visit(_elem[i]);
}

template<typename T>
template<typename VST>
void Vector<T>::traverse(VST &visit) {
    for (int i = 0; i < _size; i++) visit(_elem[i]);
}

template<typename T>
int Vector<T>::disordered() const { //统计向量中的逆序相邻元素对
    int n = 0;  //计数器
    for (int i = 0; i < _size; i++) {   //逐一检查各个相邻对
        n += (_elem[i - 1] > _elem[i]); //若逆序则计数器自增
    }
    return n;
}

//template<typename T>
//int Vector<T>::uniquify() { //低效版本
//    int oldSize = _size;
//    int i = 1;
//    while(i < _size) {  //从前向后，逐一比对各对相邻元素
//        _elem[i - 1] == _elem[i] ? remove(i) : i++; //如果相同则remove(),否则i自增前进
//    }
//    return oldSize - _size; //这里的_size的变化由remove()隐式地完成
//}

template<typename T>
int Vector<T>::uniquify() {
    Rank i = 0, j = 0;
    while(++ j < _size) {   //逐一扫描直到末元素
        //i取到相同元素的第一个元素，使j自增，当找到第一个非相同元素时，将该元素移至i后
        if(_elem[i] != _elem[j]) _elem[++i] = _elem[j];
    }
    _size = ++i;
    shrink();
    return j - i; //此时j = oldSize, i = _size
}

template<typename T>
void Vector<T>::shrink() {
    if(_capacity < DEFAULT_CAPACITY << 1) return;   //防止收缩了比初始容量小
    if(_capacity < size << 2) return;   //如果装载因子大于0.25则不收缩
    T *oldElem = _elem;
    _elem = new T[_capacity >> 1];  //容量减半
    for (int i = 0; i < ; i++) _elem[i] = oldElem[i];   //一一赋值
    delete[] oldElem;   //释放原空间
}

template<typename T>
Rank Vector<T>::search(const T &e, Rank lo, Rank hi) const {
    return (rand()%2) ? binSearch(_elem, e, lo, hi) : fibSearch(_elem, e, lo, hi);  //随机使用两种查找算法
}

template<typename T>
Rank Vector<T>::bubble(Rank lo, Rank hi) {
    Rank last = lo;
    while(++lo < hi) {
        if(_elem[lo - 1] > _elem[lo]) {
            last = lo;  //记录最后一次逆序的位置，后面的都是有序的
            swap(_elem[lo - 1], _elem[lo]);
        }
    }
}

template<typename T>
void Vector<T>::bubbleSort() {
    while(lo < (hi = bubble(lo, hi)));
}

template<typename T>
void Vector<T>::merge(Rank lo, Rank mi, Rank hi) {  //二路归并算法
    T *A = _elem + lo; //将向量的相对初始位置移到lo
    int lb = mi - lo;   //左区间记为B，长度为lb
    T *B = new T[lb];
    for (Rank i = 0; i < lb; B[i] = A[i++]);   //为左区间子向量创建新空间，并复制到新空间
    int lc = hi - mi;
    T *C = _elem + mi;  //后子向量位置
    for (Rank i = 0, j = 0, k = 0; j < lb || k < lc) {  //当左右区间有元素循环就继续
        //该循环为A也就是按序插入数值
        if(j < lb && (lc <= k || B[j] <= C[k])) A[i++] = B[j++];
        //B[j]存在不越界 && (C已经放完 || B[j]中的元素较小))
        if(k < lc && (lb <= j || C[k] < B[j])) A[i++] = C[k++];
    }
}

template<typename T>
void Vector<T>::mergeSort(Rank lo, Rank hi) {
    if(hi - lo < 2) return; //区间为无元素或单元素
    int mi = (lo + hi) >> 1;
    mergeSort(lo, mi);  //左半段
    mergeSort(mi, hi);  //右半段
    merge(lo, mi, hi);  //归并
}

template<typename T>
static void swap(T &e1, T &e2) {
    T temp = e1;
    e1 = e2;
    e2 = temp;
}

template<typename T>
static Rank binSearch(T *A, T const &e, Rank lo, Rank, hi) {
    while(lo < hi) {
        Rank mi = (lo + hi) >> 1;
        e < A[mi] ? hi = mi : lo = mi + 1; //[lo,mi)或(mi,hi)
        //如果命中，lo元素将会是带查找的元素雷同的元素区间中最后一个元素的后一位
    }//出口时,lo == hi
    return --lo;
}

template<typename T>
static Rank fibSearch(T *A, T const &e, Rank lo, Rank, hi) {
    Fib fib(hi - lo);
    while(lo < hi) {
        while(hi - lo < fib.get()) fib.prev();  //通过前序查找找到分割点
        Rank mi = lo + fib.get() - 1;
        if(e < A[mi]) hi = mi;  //深入前半段
        else if(A[mi] < e) lo = mi + 1; //深入后半段
        else return mi; //命中
    }
    return -1; //查找失败
}








