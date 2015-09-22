#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdint>
#include <stdexcept>
#include <utility>

//Utility gives std::rel_ops which will fill in relational
//iterator operations so long as you provide the
//operators discussed in class.  In any case, ensure that
//all operations listed in this website are legal for your
//iterators:
//http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
using namespace std::rel_ops;

namespace epl{
    class invalid_iterator {
    public:
        enum SeverityLevel {SEVERE,MODERATE,MILD,WARNING};
        SeverityLevel level;
        
        invalid_iterator(SeverityLevel level = SEVERE){ this->level = level; }
        virtual const char* what() const {
            switch(level){
                case WARNING:   return "Warning"; // not used in Spring 2015
                case MILD:      return "Mild";
                case MODERATE:  return "Moderate";
                case SEVERE:    return "Severe";
                default:        return "ERROR"; // should not be used
            }
        }
    };
    
    template <typename T>
    class vector {
    private:
        T* head;
        T* first;
        T* last;
        uint64_t length;
        uint64_t capacity;
        
        uint64_t version{0};
        uint64_t assignmentversion{0};
        uint64_t push_fronts{0};
        uint64_t pop_fronts{0};
        
        static constexpr uint64_t mincapacity = 8;
        
    public:
        class iterator;
        class const_iterator;
        
        class iterator {
        private:
            vector<T>* container;//tell the iterator where it comes from
            int64_t index;
            uint64_t version;
            
            T* head;
            uint64_t push_fronts;
            uint64_t pop_fronts;
            uint64_t assignmentversion;
            bool invalid{false};
        public:
            //define all member types
            using value_type = T;
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = int64_t;// or std::ptrdiff_t?
            using pointer = T*;
            using reference = T&;
            
            //check whether the iterator is valid
            void checkvalidation(bool dereference = false) {
                    int64_t index = this->index;
                if (this->version != this->container->version) {
                    if ((this->invalid == false) && (index < 0 || index >= this->container->length)) {
                        throw invalid_iterator{invalid_iterator::SEVERE};
                    } else if ((index >= 0 && index < this->container->length) && (this->assignmentversion != this->container->assignmentversion || this->head != this->container->head)) {
                        throw invalid_iterator{invalid_iterator::MODERATE};
                    } else {
                        throw invalid_iterator{invalid_iterator::MILD};
                    }
                }
            }
            //the const version of checkvalidation
            void checkvalidation(bool dereference = false) const {
                int64_t index = this->index;
                if (this->version != this->container->version) {
                    //invalid == false means the iteator is initially valid
                    if ((this->invalid == false) && (index < 0 || index >= this->container->length)) {
                        throw invalid_iterator{invalid_iterator::SEVERE};
                    } else if ((index >= 0 && index < this->container->length) && (this->assignmentversion != this->container->assignmentversion || this->head != this->container->head)) {
                        throw invalid_iterator{invalid_iterator::MODERATE};
                    } else {
                        throw invalid_iterator{invalid_iterator::MILD};
                    }
                }
            }
            
            
            //iterator constructors
            iterator(vector<T>* v, int64_t index) {
                this->container = v;
                this->index = index;
                this->version = v->version;
                this->head = v->head;
                this->assignmentversion = v->assignmentversion;
                this->push_fronts = v->push_fronts;
                this->pop_fronts = v->pop_fronts;
                if (index < 0 || index >= v->length) {
                    invalid = true;
                }
            }
            //iterator cast
            operator const_iterator() {
                return const_iterator(container, index);
            }
            
            //copy constructor for iterator
            iterator(const iterator& that) {
                copy_iterator(that);
            }
            //copy assignment for iterator
            iterator& operator=(const iterator& that) {
                that.checkvalidation();
                if (this != &that) {
                    //destroy_iterator();
                    copy_iterator(that);
                }
                return *this;
            }
            
            //dereference
            T& operator*(void) {
                checkvalidation(true);
                return container->operator[](index);//how about using first[index]?
            }
            
            //relational opertator
            //operator <
            bool operator<(const iterator& that) const {
                checkvalidation();
                that.checkvalidation();
                return (this->index < that.index);
            }
            
            //operator ==
            bool operator==(const iterator& that) const {
                checkvalidation();
                that.checkvalidation();
                return (this->index == that.index);
            }
            /*
            bool operator!=(const iterator& that) const {//need to check?
                checkvalidation();
                that.checkvalidation();
                const iterator& lhs = *this;
                return !(lhs == that);
            }
            //operator >
            bool operator>(const iterator& that) const {//need to check?
                checkvalidation();
                that.checkvalidation();
                const iterator& lhs = *this;
                return that < lhs;
            }
            bool operator>=(const iterator& that) const {//need to check?
                checkvalidation();
                that.checkvalidation();
                const iterator& lhs = *this;
                return !(lhs < that);
            }
            bool operator<=(const iterator& that) const {//need to check?
                checkvalidation();
                that.checkvalidation();
                const iterator& lhs = *this;
                return !(that < lhs);
            }
             */
             
            //pre-increment
            iterator& operator++(void) {
                checkvalidation();
                ++index;
                return *this;
            }
            
            iterator operator++(int) {//need to check?
                checkvalidation();
                iterator i{*this};
                this->operator++();
                return i;
            }
            //pre-decrement
            iterator& operator--(void) {
                checkvalidation();
                --index;
                return *this;
            }
            
            iterator operator--(int) {//need to check?
                checkvalidation();
                iterator i{*this};
                this->operator--();
                return i;
            }
            
            iterator operator+(int64_t n) {
                checkvalidation();
                return iterator{container, index + n};
            }
            
            friend const iterator operator+(int64_t n, iterator& lhs) {//need to check?
                return lhs + n;
            }
            
            iterator operator-(int64_t n) {
                checkvalidation();
                return iterator{container, index - n};
            }
            
            int64_t operator-(const iterator& that) {
                checkvalidation();
                that.checkvalidation();
                return this->index - that.index;
            }
            
            iterator& operator+=(int64_t n) {
                checkvalidation();
                index += n;
                return *this;
            }
            
            iterator& operator-=(int64_t n) {
                checkvalidation();
                index -= n;
                return *this;
            }
            
            T& operator[](int64_t n) {
                checkvalidation(true);
                return container->operator[](index + n);
            }
            /*
             ~iterator(void) {
             destroy_iterator();
             }*/
        private:
            //void destroy_iterator();
            void copy_iterator(const iterator& that) {
                this->container = that.container;
                this->index = that.index;
                this->version = that.version;
                this->head = that.head;
                this->push_fronts = that.push_fronts;
                this->pop_fronts = that.pop_fronts;
                this->assignmentversion = that.assignmentversion;
                this->invalid = that.invalid;
                
            }
        };// the end of the normal iterator
        
        
        class const_iterator {
        private:
            const vector<T>* container;//tell the iterator where it comes from
            int64_t index;
            uint64_t version;
            
            T* head;
            uint64_t push_fronts;
            uint64_t pop_fronts;
            uint64_t assignmentversion;
            bool invalid{false};
        public:
            //define all member types
            using value_type = T;
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = int64_t;// or std::ptrdiff_t?
            using pointer = T*;
            using reference = T&;
            
            //check whether the iterator is valid
            void checkvalidation(bool dereference = false) {
                int64_t index = this->index;
                if (this->version != this->container->version) {
                    
                    if ((this->invalid == false) && (index < 0 || index >= this->container->length)) {
                        throw invalid_iterator{invalid_iterator::SEVERE};
                    } else if ((index >= 0 && index < this->container->length) && (this->assignmentversion != this->container->assignmentversion || this->head != this->container->head)) {
                        throw invalid_iterator{invalid_iterator::MODERATE};
                    } else {
                        throw invalid_iterator{invalid_iterator::MILD};
                    }
                }
            }
            //the const version of checkvalidation
            void checkvalidation(bool dereference = false) const {
                int64_t index = this->index;
                if (this->version != this->container->version) {
                    if ((this->invalid == false) && (index < 0 || index >= this->container->length)) {
                        throw invalid_iterator{invalid_iterator::SEVERE};
                    } else if ((index >= 0 && index < this->container->length) && (this->assignmentversion != this->container->assignmentversion || this->head != this->container->head)) {
                        throw invalid_iterator{invalid_iterator::MODERATE};
                    } else {
                        throw invalid_iterator{invalid_iterator::MILD};
                    }
                }
            }
            
            //const_iterator constructors
            const_iterator(const vector<T>* v, int64_t index) {
                this->container = v;
                this->index = index;
                this->version = v->version;
                this->head = v->head;
                this->assignmentversion = v->assignmentversion;
                this->push_fronts = v->push_fronts;
                this->pop_fronts = v->pop_fronts;
                if (index < 0 || index >= v->length) {
                    invalid = true;
                }
            }
            //iterator cast
            
            //copy constructor for iterator
            const_iterator(const const_iterator& that) {
                copy_const_iterator(that);
            }
            //copy assignment for iterator
            const_iterator& operator=(const const_iterator& that) {
                that.checkvalidation();
                if (this != &that) {
                    //destroy_iterator();
                    copy_const_iterator(that);
                }
                return *this;
            }
            
            //dereference
            const T& operator*(void) {
                checkvalidation(true);
                return container->operator[](index);//how about using first[index]?
            }
            
            //relational opertator
            //operator <
            bool operator<(const const_iterator& that) const {
                checkvalidation();
                that.checkvalidation();
                return (this->index < that.index);
            }
            
            //operator ==
            bool operator==(const const_iterator& that) const {
                checkvalidation();
                that.checkvalidation();
                return (this->index == that.index);
            }
            /*
            bool operator!=(const iterator& that) const {//need to check?
                checkvalidation();
                that.checkvalidation();
                const iterator& lhs = *this;
                return !(lhs == that);
            }
            //operator >
            bool operator>(const iterator& that) const {//need to check?
                checkvalidation();
                that.checkvalidation();
                const iterator& lhs = *this;
                return that < lhs;
            }
            bool operator>=(const iterator& that) const {//need to check?
                checkvalidation();
                that.checkvalidation();
                const iterator& lhs = *this;
                return !(lhs < that);
            }
            bool operator<=(const iterator& that) const {//need to check?
                checkvalidation();
                that.checkvalidation();
                const iterator& lhs = *this;
                return !(that < lhs);
            }
             */
            //pre-increment
            const_iterator& operator++(void) {
                checkvalidation();
                ++index;
                return *this;
            }
            
            const_iterator operator++(int) {//need to check?
                checkvalidation();
                const_iterator i{*this};
                this->operator++();
                return i;
            }
            //pre-decrement
            const_iterator& operator--(void) {
                checkvalidation();
                --index;
                return *this;
            }
            
            const_iterator operator--(int) {//need to check?
                checkvalidation();
                const_iterator i{*this};
                this->operator--();
                return i;
            }
            
            const_iterator operator+(int64_t n) {
                checkvalidation();
                return const_iterator{container, index + n};
            }
            
            friend const iterator operator+(int64_t n, iterator& lhs) {//need to check?
                return lhs + n;
            }
            
            const_iterator operator-(int64_t n) {
                checkvalidation();
                return const_iterator{container, index - n};
            }
            
            int64_t operator-(const const_iterator& that) {
                checkvalidation();
                that.checkvalidation();
                return this->index - that.index;
            }
            
            const_iterator& operator+=(int64_t n) {
                checkvalidation();
                index += n;
                return *this;
            }
            
            const_iterator& operator-=(int64_t n) {
                checkvalidation();
                index -= n;
                return *this;
            }
            
            const T& operator[](int64_t n) {
                checkvalidation(true);
                return container->operator[](index + n);
            }
            /*
             ~iterator(void) {
             destroy_iterator();
             }*/
        private:
            //void destroy_iterator();
            
            void copy_const_iterator(const const_iterator& that) {
                this->container = that.container;
                this->index = that.index;
                this->version = that.version;
                this->head = that.head;
                this->push_fronts = that.push_fronts;
                this->pop_fronts = that.pop_fronts;
                this->assignmentversion = that.assignmentversion;
                this->invalid = that.invalid;
                
            }
        };// the end of the cosntant iterator
        
        iterator begin(void) {//when should we using iterator&?
            if (first == head && last == head - 1) {// the vector is empty
                return iterator(this, -1);//if it is empty, what should we return?
            } else {
                return iterator(this, 0);//maybe a problem?
            }
        }
        
        
        
        iterator end(void) {
            if (first == head && last == head - 1) {// the vector is empty
                return iterator(this, -1);//if it is empty, what should we return?
            } else {
                return iterator(this, length);//maybe a problem?
            }
        }
        
        const_iterator begin(void) const {
            if (first == head && last == head - 1) {// the vector is empty
                return const_iterator(this, -1);//if it is empty, what should we return?
            } else {
                return const_iterator(this, 0);//maybe a problem?
            }
        }
        
        const_iterator end(void) const {
            if (first == head && last == head - 1) {// the vector is empty
                return const_iterator(this, -1);//if it is empty, what should we return?
            } else {
                return const_iterator(this, length);//maybe a problem?
            }
        }
        
        template <typename... Args>
        void emplace_back(Args&&... args) {
            if (last - head == capacity - 1) {//need reallocation
                T* newhead = (T*)operator new(sizeof(T) * 2 * capacity);
                uint64_t pace = first - head;
                last = newhead + pace + length - 1;
                last++;
                new(last) T{std::forward<Args...>(args...)};//maybe a problem?
                for (int i = 0; i < length; i++) {
                    new(newhead + i + pace) T{std::move(first[i])};
                }
                for (int i = 0; i < length; i++) {
                    first[i].~T();
                }
                
                operator delete(head);
                capacity *= 2;
                head = newhead;
                first = pace + head;
                length++;
                
            } else {
                last++;
                new(last) T{std::forward<Args...>(args...)};//too much elments, vector can't contain?
                length++;// how to reset my lenth?
            }
        }
        
        template <typename RAI>
        void iterator_initialize_vector(RAI b, RAI e, std::random_access_iterator_tag t) {
            uint64_t size = e - b;//do we need to initialize e?
            if (size == 0) {
                head = (T*)operator new(sizeof(T)*mincapacity);
                first = head;
                last = head - 1;
                capacity = mincapacity;
                length = 0;
            } else {
                head = (T*) operator new(sizeof(T)* size);
                for (int i = 0; i < size; i++) {
                    new(head + i) T{b[i]};
                }
                first = head;
                last = first + size - 1;
                capacity = size;
                length = size;
            }
        }
        
        template <typename FI>
        void iterator_initialize_vector(FI b, FI e, std::input_iterator_tag t) {
            head = (T*)operator new(sizeof(T)*mincapacity);
            first = head;
            last = head - 1;
            capacity = mincapacity;
            length = 0;
            
            while (b != e) {
                push_back(*b);
                ++b;
            }
        }
        
        /*
         //how to write the template constructor using [b,e)?
         template <typename I>
         struct Iterator_traits {
         typedef typename I::value_type value_type;
         using iterator_category = typename I::iterator_category;
         };
         //don't need the following code?
         template <typename I>
         struct Iterator_traits<I*> {
         using value_type = I;
         using iterator_category = std::random_access_iterator_tag;
         };
         */
        /*
         template <typename Iterator>
         vector(Iterator b, Iterator e) {
         typename Iterator_traits<Iterator>::iterator_category x{};
         iterator_initial_vector(b, e, x);
         }*/
        
        template <typename Iterator>
        vector(Iterator b, Iterator e) {
            typename std::iterator_traits<Iterator>::iterator_category x{};
            iterator_initialize_vector(b, e, x);
        }
        
        vector(std::initializer_list<T> list) {
            if (list.size() == 0) {
                head = (T*)operator new(sizeof(T)*mincapacity);
                first = head;
                last = head - 1;
                capacity = mincapacity;
                length = 0;
            } else {
                auto p = list.begin();
                head = (T*) operator new(sizeof(T)* list.size());
                for (int i = 0; i < list.size(); i++) {
                    new(head + i) T{std::move(p[i])};
                }
                first = head;
                last = first + list.size() - 1;
                capacity = list.size();
                length = list.size();
            }
        }
        
        //non-argument constructor for vector
        vector(void) {
            head = (T*)operator new(sizeof(T)*mincapacity);
            first = head;
            last = head - 1;
            capacity = mincapacity;
            length = 0;
        }
        
        explicit vector(uint64_t n) {
            if (n == 0) {
                head = (T*) operator new(sizeof(T)*mincapacity);
                first = head;
                last = head - 1;
                capacity = mincapacity;
                length = 0;
            } else {
                head = (T*) operator new(sizeof(T)* n);
                for (int i = 0; i < n; i++) {
                    new(head + i) T{};
                }
                first = head;
                last = first + n - 1;
                capacity = n;
                length = n;
            }
        }
        //copy constructor
        vector(const vector<T>& that) {
            copy(that);
        }
        //move constructor
        vector(vector<T>&& that) {
            move(std::move(that));
            //move(that); //why this would be an error
            that.version += 1;
        }
        
        //move asssignment
        vector<T>& operator=(vector<T>&& that) {
            if (this != &that) {
                destroy();
                move(std::move(that));
            }
            //both verctors have changed their states
            this->version += 1;
            that.version += 1;
            assignmentversion += 1;
            
            return *this;
        }//maybe some problems
        
        //copy assignment
        vector<T>& operator=(const vector<T>& that) {//maybe something wrong?
            if (this != &that) {
                destroy();
                copy(that);
            }
            
            this->version += 1;
            assignmentversion += 1;
            return *this;
        }
        
        ~vector(void) {
            destroy();
        }
        
        uint64_t size(void) const {
            return length;
        }
        
        T& operator[](uint64_t k) {
            if (k < 0 || first + k > last) {
                throw std::out_of_range("subscript out of range");
            } else {
                return first[k];
            }
        }
        
        const T& operator[](uint64_t k) const {
            if (k < 0 || first + k > last) {
                throw std::out_of_range("subscript out of range");
            } else {
                return first[k];
            }
        }
        
        //push_back copy construct
        void push_back(const T& data) {
            if (last - head == capacity - 1) {
                T* newhead = (T*)operator new(sizeof(T) * 2 * capacity);
                uint64_t pace = first - head;
                last = newhead + pace + length - 1;
                last++;
                new(last) T{data};
                for (int i = 0; i < length; i++) {
                    new(newhead + i + pace) T{std::move(first[i])};
                }
                for (int i = 0; i < length; i++) {
                    first[i].~T();
                }
                
                operator delete(head);
                capacity *= 2;
                head = newhead;
                first = pace + head;
                length++;
                
            } else {
                last++;
                new(last) T{data};//copy constructor
                length++;
            }
            
            this->version += 1;
        }
        
        //push_back move construct
        void push_back(T&& data) {
            if (last - head == capacity - 1) {
                T* newhead = (T*)operator new(sizeof(T) * 2 * capacity);
                uint64_t pace = first - head;
                last = newhead + pace + length - 1;
                last++;
                new(last) T{std::move(data)};
                for (int i = 0; i < length; i++) {
                    new(newhead + i + pace) T{std::move(first[i])};
                }
                for (int i = 0; i < length; i++) {
                    first[i].~T();
                }
                
                operator delete(head);
                capacity *= 2;
                head = newhead;
                first = pace + head;
                length++;
                
            } else {
                last++;
                new(last) T{std::move(data)};//copy constructor
                length++;
            }
            this->version += 1;
        }
        
        //push_front copy construct
        void push_front(const T& data) {
            if (first == head && last == head - 1) {//the vector is empty
                new(first) T{data};
                last++;
                length++;
            } else {
                if (first == head) {//the vector's front capacity is 0
                    T* newhead = (T*)operator new(sizeof(T) * capacity * 2);
                    new(newhead + capacity - 1) T{data};
                    for (int i = 0; i < length; i++) {
                        new(newhead + i + capacity) T{std::move(first[i])};
                    }
                    
                    for (int i = 0; i < length; i++) {
                        first[i].~T();
                    }
                    operator delete(head);
                    head = newhead;
                    first = head + capacity - 1;
                    last = first + length - 1;
                    last++;
                    capacity *= 2;
                    length++;
                } else if (first > head) {//the vector has available front capacity
                    first--;
                    new(first) T{data};
                    length++;
                }
            }
            this->version += 1;
            push_fronts++;
        }
        //push_front move contruct
        void push_front(T&& data) {
            if (first == head && last == head - 1) {//the vector is empty
                new(first) T{std::move(data)};
                last++;
                length++;
            } else {
                if (first == head) {//the vector's front capacity is 0
                    T* newhead = (T*)operator new(sizeof(T) * capacity * 2);
                    new(newhead + capacity - 1) T{std::move(data)};
                    for (int i = 0; i < length; i++) {
                        new(newhead + i + capacity) T{std::move(first[i])};
                    }
                    
                    for (int i = 0; i < length; i++) {
                        first[i].~T();
                    }
                    operator delete(head);
                    head = newhead;
                    first = head + capacity - 1;
                    last = first + length - 1;
                    last++;
                    capacity *= 2;
                    length++;
                } else if (first > head) {//the vector has available front capacity
                    first--;
                    new(first) T{std::move(data)};
                    length++;
                }
            }
            this->version += 1;
            push_fronts++;
        }
        
        void pop_back(void) {
            if (this->size() == 0) {
                throw std::out_of_range("array is empty");
            } else {
                last->~T();
                last--;
                length--;
            }
            this->version += 1;
        }
        
        void pop_front(void) {
            if (this->size() == 0) {
                throw std::out_of_range("array is empty");
            } else {
                first->~T();
                first++;
                length--;
            }
            this->version += 1;
            pop_fronts++;
        }
    private:
        void copy(const vector<T>& that) {
            this->capacity = that.capacity;//test that.capacity==0?
            this->length = that.length;
            head = (T*)operator new(sizeof(T)*that.capacity);
            first = head + (that.first - that.head);
            last = head + (that.last - that.head);
            for (int i = 0; i < that.length; i++) {//i<that.capacity or i<that.length?
                new(first + i) T{that.first[i]};
            }
        }
        
        void move(vector<T>&& that) {
            this->capacity = that.capacity;
            this->length = that.length;
            this->head = that.head;
            this->first = that.first;
            this->last = that.last;
            that.head = nullptr;
            that.first = nullptr;
            that.last = nullptr;
            that.capacity = 0;
            that.length = 0;
        }
        
        void destroy(void) {
            for (int i = 0; i < this->length; i++) {
                first[i].~T();
            }
            operator delete(head);
            
        }
    };
    
} //namespace epl

#endif /* _Vector_h */
