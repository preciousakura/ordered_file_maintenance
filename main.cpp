#include <iostream>
#include <stdio.h>
#include <utility>
#include <bits/stdc++.h> 

#define SIZE 8

using namespace std;

class OrderedFile {
  private:
    typedef struct Data {
      bool is_null;
      int value;

      Data(): is_null(true) {}
    } Data;

    typedef struct List {
      int size = SIZE;
      Data *values;

      List(): values(new Data[SIZE]) {}

      int get_range() { return log2(this->size); }

      int get_height() { return floor(log2(this->size/this->get_range())); }

      int get_n(int begin, int end) {
        int n = 0;
        for(int i = begin; i <= end; i++) 
          n += (!this->values[i].is_null);
        return n;
      }

      double get_density(int begin, int end, int n) { return ((double) n/(end - begin + 1)); }

      pair<double, double> get_density_bound(int d) {
        pair<double, double> bound;
        int h = this->get_height();
        bound.first = 0.5 - (0.25 * (d/h));
        bound.second = 0.75 + (0.25 * (d/h));
        return bound;
      }
      
      void shift_left(int pos){
        Data value = this->values[pos];
        while(!this->values[--pos].is_null) {
          Data aux = this->values[pos];
          this->values[pos] = value;
          value = aux;
        }
        this->values[pos] = value;
      }

      void shift_right(int pos) {
        Data value = this->values[pos];
        while(!this->values[++pos].is_null) {
          Data aux = this->values[pos];
          this->values[pos] = value;
          value = aux;
        }
        this->values[pos] = value;
      }

      void change_size(int size) {
        int j = 0;
        size = size < SIZE ? SIZE : size;
        Data *items = new Data[size];
        
        for(int i = 0; i < this->size; i++) {
          if(!this->values[i].is_null) {
            items[j] = this->values[i];
            items[j].is_null = false;
            j++;
          }
        }

        delete[] this->values;
        this->size = size;
        this->values = items;
      }

      void print() {
        for(int i = 0; i < size; i++) {
          if(this->values[i].is_null) cout << "//" << ' ';
          else cout << this->values[i].value << ' ';
        }
        cout << endl;
      }

      void print(int begin, int end) {
        for(int i = begin; i <= end; i++) {
          if(this->values[i].is_null) cout << "//" << ' ';
          else cout << this->values[i].value << ' ';
        }
        cout << endl;
      }
    } List;

    List list;

    int binary_search(int x) {
      int low = 0, high = this->list.size - 1, mid = 0;
      while(low <= high) {
        mid = low + (high - low) / 2; 

        while(this->list.values[mid].is_null && mid <= high) mid++;

        if(mid > high) {
          mid = low + (high - low) / 2; 
          while(this->list.values[mid].is_null && mid >= low) mid--;
        }

        if(this->list.values[mid].value == x) return mid;
        if(mid < low) return low;
        else if(this->list.values[mid].value < x) low = mid + 1;
        else high = mid - 1;
      }
      return low;
    }

    void redistribute(int begin, int end) {
      end = end >= this->list.size ? this->list.size - 1 : end;

      int n = this->list.get_n(begin, end);
      Data items[n];
      int j = 0;

      for(int i = begin; i <= end; i++) {
        if(!this->list.values[i].is_null) {
          items[j] = this->list.values[i];
          this->list.values[i].is_null = true;
          j++;
        }
      }

      int range = this->list.get_range();
      int pos = begin, n_values = 0;
      int depth = this->list.get_height();
      pair<double, double> bound = this->list.get_density_bound(depth);

      for(int i = 0; i < j; i++) {
        this->list.values[pos + n_values] = items[i];
        this->list.values[pos + n_values].is_null = false;
        n_values++;

        if(pos == (this->list.size - 2) && (range & 1)) range = 2;
        double density = ((double) (n_values + 1.0)/(range));

        if(density < bound.first || density >= bound.second) { 
          pos += range;
          n_values = 0;
        }
      }
    }
    
    void scan(int begin, int end, int depth, bool upper_limit) {
      double density = this->list.get_density(begin, end, this->list.get_n(begin, end));
      pair<double, double> bound = this->list.get_density_bound(depth);
      int size = end - begin + 1;
      bool out_bounds =  upper_limit ? density > bound.second : density < bound.first;

      if(size >= this->list.size && out_bounds) {
        this->list.change_size(upper_limit ? this->list.size * 2 : this->list.size / 2);
        begin = 0; end = this->list.size - 1;
      }
      else {
        int range = this->list.get_range();
        if(out_bounds) { 
          depth -= 1;
          if((begin == this->list.size - 2) && (range & 1)) {
            begin -= 2 * range; depth -= 1; 
          }
          int index = begin / range;
          if(index & 1) begin -= size; 
          else {
            end = (end + size) >= this->list.size ? this->list.size - 1 : end + size;
          }
          scan(begin, end, depth, upper_limit);
        } 
      }

      redistribute(begin, end);
    }

    void start_scan(int pos, bool upper_limit) {
      // int range = this->list.get_range();
      // int begin =  pos - (pos % range); 
      // int depth = this->list.get_height();
      // int end = (begin + range - 1) >= this->list.size ? this->list.size - 1 : begin + range - 1;

      // double density = this->list.get_density(begin, end, this->list.get_n(begin, end) + 1);
      // pair<double, double> bound = this->list.get_density_bound(depth);
      // bool out_bounds =  upper_limit ? density > bound.second : density < bound.first;

      // if(out_bounds) 
      //   scan(begin, end, depth, upper_limit);
    }

  public:
    void insert(int x) {
      Data item;
      item.value = x;
      item.is_null = false;

      if(this->list.get_n(0, this->list.size - 1) == 0) {
        this->list.values[0] = item;
        return;
      }

      if((this->list.get_n(0, this->list.size - 1) + 1) > this->list.size) {
        this->list.change_size(this->list.size * 2);
      }

      int pos = binary_search(x);   
      int range = this->list.get_range();
      int begin =  pos - (pos % range); 
      int end = (begin + range - 1) >= this->list.size ? this->list.size - 1 : begin + range - 1;
      double density = this->list.get_density(begin, end, this->list.get_n(begin, end));

      if(this->list.values[pos].is_null) {
        this->list.values[pos] = item;
      }
      else {
        if(this->list.values[pos].value > x) 
          this->list.shift_right(pos);
        else 
          this->list.shift_left(pos);
        this->list.values[pos] = item;
      }

      int depth = this->list.get_height();
      pair<double, double> bound = this->list.get_density_bound(depth);
      bool out_bounds =  density > bound.second || density < bound.first;

      if(out_bounds) 
        scan(begin, end, depth, true);
    }

    void remove(int x){
      int pos = binary_search(x);
      Data value = this->list.values[pos];
      if(!value.is_null && value.value == x) {
        this->list.values[pos].is_null = true;
        start_scan(pos, false);
      }
    }
    
    void print() { this->list.print(); }
};

int main() {
  OrderedFile file;

  file.insert(1);
  file.print();
  file.insert(2);
  file.print();
  file.insert(4);
  file.print();
  file.insert(3);
  file.print();
  file.insert(5);
  file.print();
  file.insert(6);
  file.print();
  file.insert(7);
  file.print();
  file.insert(8);
  file.print();
  file.insert(9);
  file.print();

  return 0;
}