#include <iostream>
#include <stdio.h>
#include <utility>
#include <bits/stdc++.h> 
#include <fstream>
#include <sstream>
#include <string.h>

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

      int get_range() { 
        int log_n = log2(this->size);
        int n_leafs = ceil((double)this->size / (double)log_n);
        return pow(2,(int)round(log2(n_leafs)));
      }

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

      void print(ofstream& file) {
        for(int i = 0; i < size; i++) 
          if(!this->values[i].is_null) 
            file << this->values[i].value << ' ';
        file << endl;
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
      int n = this->list.get_n(begin, end);
      int blank_space = (end - begin + 1) - n;

      Data items[n];
      int j = 0;

      for(int i = begin; i <= end; i++) {
        if(!this->list.values[i].is_null) {
          items[j] = this->list.values[i];
          this->list.values[i].is_null = true;
          j++;
        }
      }

      int pos = begin;
      int block = (n/blank_space) + 1;
      for(int i = 0; i < j; i++) {
        if(i > block - 1 && i%block == 0) pos++;
        this->list.values[pos] = items[i];
        pos++;
      }
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

      int pos = binary_search(x);   

      if(this->list.values[pos].is_null) {
        this->list.values[pos] = item;
        return;
      }

      int range = this->list.get_range();
      int begin =  pos - (pos % range); 
      int end = (begin + range - 1) >= this->list.size ? this->list.size - 1 : begin + range - 1;

      int end_total = end;
      int begin_total = begin;

      int size = end - begin + 1;
      int element_count = this->list.get_n(begin, end) + 1;
      int leaf_idx = begin / range;
      int range_increase = 1;
      
      double density = this->list.get_density(begin, end, element_count);
      int depth = this->list.get_height();
      pair<double, double> bound = this->list.get_density_bound(depth);
      bool out_bounds =  density > bound.second || density < bound.first;

      if(!out_bounds) {
        if(this->list.values[pos].value > x) 
          this->list.shift_right(pos);
        else 
          this->list.shift_left(pos);
        this->list.values[pos] = item;
        return;
      }

      while(out_bounds && !((end_total - begin_total + 1) == this->list.size)) {
        if (leaf_idx & 1) {
          end = begin - 1;
          begin = begin - range_increase * range;
        }
        else {
          begin = end + 1;  
          end = end + range_increase * range;
          if(end > this->list.size - 1) 
            end = this->list.size - 1;
        }

        range_increase *= 2;
        leaf_idx = leaf_idx >> 1;
        begin_total =  begin < begin_total ? begin : begin_total;
        end_total =  end > end_total ? end : end_total;

        size += end - begin + 1; 
        element_count += this->list.get_n(begin, end);
        density = ((double) element_count / size); 
        depth--;

        bound = this->list.get_density_bound(depth);
        out_bounds =  density > bound.second || density < bound.first;
      }

      if (((end_total - begin_total + 1) == this->list.size) && out_bounds) {
        this->list.change_size(this->list.size * 2);
        end_total = this->list.size - 1;
      }

      if (this->list.values[pos].is_null) 
        this->list.values[pos] = item;
      else if(this->list.values[pos].value > x) 
        this->list.shift_right(pos);
      else 
        this->list.shift_left(pos);
      this->list.values[pos] = item;
      redistribute(begin_total, end_total);
    }

    void remove(int x) {
      int pos = binary_search(x); 
      if(this->list.values[pos].value != x) return;

      this->list.values[pos].is_null = true;

      int range = this->list.get_range();
      int begin =  pos - (pos % range); 
      int end = (begin + range - 1) >= this->list.size ? this->list.size - 1 : begin + range - 1;

      int end_total = end;
      int begin_total = begin;

      int size = end - begin + 1;
      int element_count = this->list.get_n(begin, end);
      int leaf_idx = begin / range;
      int range_increase = 1;
      
      double density = this->list.get_density(begin, end, element_count);
      int depth = this->list.get_height();
      pair<double, double> bound = this->list.get_density_bound(depth);
      bool out_bounds =  density > bound.second || density < bound.first;

      if(!out_bounds) return;

      while(out_bounds && !((end_total - begin_total + 1) == this->list.size)) {
        if (leaf_idx & 1) {
          end = begin - 1;
          begin = begin - range_increase * range;
        }
        else {
          begin = end + 1;  
          end = end + range_increase * range;
          if(end > this->list.size - 1) 
            end = this->list.size - 1;
        }

        range_increase *= 2;
        leaf_idx = leaf_idx >> 1;
        begin_total =  begin < begin_total ? begin : begin_total;
        end_total =  end > end_total ? end : end_total;

        size += end - begin + 1; 
        element_count += this->list.get_n(begin, end);
        density = ((double) element_count / size); 
        depth--;

        bound = this->list.get_density_bound(depth);
        out_bounds =  density > bound.second || density < bound.first;
      }

      if (((end_total - begin_total + 1) == this->list.size) && out_bounds) {
        this->list.change_size(this->list.size/2);
        end_total = this->list.size - 1;
      }

      redistribute(begin_total, end_total);
    }

    int successor(int x) { 
      int pos = binary_search(x); 
      if(this->list.values[pos].value == x) 
        while(this->list.values[pos++].is_null);
      
      if(this->list.values[pos].is_null) return 99999;
      return this->list.values[pos].value;
    }
    void print() { this->list.print(); }
    void print(ofstream& file) { this->list.print(file); }
};

int main() {
  OrderedFile orderedFile;

  ifstream file("./test/2.txt");
  ofstream output_file("out.txt");

  if (file.is_open() && output_file.is_open()) {    
    string line;
    while (getline(file, line)) {
      istringstream iss(line); 
      string command; 
      iss >> command; 
      if (command == "INC") {
        int number;
        iss >> number;
        cout << "INC " << number << endl;
        orderedFile.insert(number);
      }
      else if(command == "REM") {
        int number;
        iss >> number;
        cout << "REM " << number <<endl;
        orderedFile.remove(number);
      }
      else if (command == "SUC") {
        int number;
        iss >> number;
        cout << "SUC " << number << endl;
        output_file << orderedFile.successor(number) << endl;
      }
      else if(command == "IMP") {
        cout << "IMP" << endl;
        orderedFile.print(output_file);
      }
    }
    file.close();
    output_file.close();
  }
  return 0;
}