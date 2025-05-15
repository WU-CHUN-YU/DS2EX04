// 資訊二乙 11227220張季平、11227235吳君友
#include <array>
#include <fstream>
#include <iomanip>  // 控制輸出至小數點後第幾位
#include <iostream>
#include <string>
#include <vector>
// Bin檔中的資料型態
struct DataType {
  std::array<char, 12> send_id{};     // 發訊者學號
  std::array<char, 12> receive_id{};  // 收訊者學號
  float weight;                       // 量化權重
};
// 頂點中的串列的資料型態
struct ListType {
  std::string receive_id;  // 收訊者學號
  float weight;            // 量化權重
};

// 頂點中的資料型態
struct VertexType {
  std::string send_id;                  // 發訊者學號
  std::vector<ListType> adjacent_list;  // 互動關係的陣列
};

struct VertexTypeConnect {
  std::string send_id;                  // 發訊者學號
  std::vector<ListType> adjacent_list;  // 互動關係的陣列
  int connect;             // 連通多少節點
};   

// 哈希表的資料型態
struct HashType {
  std::vector<VertexType> vertex_list;
};
//定義哈希表的一些功能
class Hash {
  // 外部可讀寫的變數
 public:
  std::vector<HashType> list;
  // 外部可使用的函式
 public:
  Hash() {
    list.resize(109);  // 設109原因是計算索引值公式為發訊者學號每位數相加，12 * 9 + 1 = 109
  }
  // 計算索引值
  int CalculateHashValue(std::string id) {
    int index = 0;
    for (int i = 0; i < 12 && id[i] != '\0'; i++) {
      index += id[i] - '0';
    }
    return index;
  }
  // 這個函式是要用來確定收訊者學號是否在列表中 //這邊投入的ID是收訊者ID
  void CheckReceiveIDIsExist(std::string id) {
    // 計算索引值
    int index = CalculateHashValue(id);
    VertexType vertex;
    vertex.send_id = id;
    if (list[index].vertex_list.empty()) {
      list[index].vertex_list.push_back(vertex);
      return;
    } else {
      // 這個For迴圈用意是尋找輸入ID是否已經存入過hash中
      for (int n = 0; n < list[index].vertex_list.size(); n++) {
        if (list[index].vertex_list[n].send_id == id) {
          return;
        }
      }
      list[index].vertex_list.push_back(vertex);
      return;
    }
  }
  // 這個函式是要用來找到發送者學號在列表中位置並更新它的Adjacent_List
  int UpdateVertexAdjacentList(std::string send_id, ListType new_list, int& index) {
    index = CalculateHashValue(send_id);
    if (list[index].vertex_list.empty()) {
      VertexType vertex;
      vertex.send_id = send_id;
      vertex.adjacent_list.push_back(new_list);
      list[index].vertex_list.push_back(vertex);
      return 0;
    } else {
      // 這個For迴圈用意是尋找輸入ID是否已經存入過hash中
      for (int n = 0; n < list[index].vertex_list.size(); n++) {
        if (list[index].vertex_list[n].send_id == send_id) {
          // 這裡是直接由小到排序插入
          for (int i = 0; i < list[index].vertex_list[n].adjacent_list.size(); i++) {
            if (new_list.receive_id < list[index].vertex_list[n].adjacent_list[i].receive_id) {
              list[index].vertex_list[n].adjacent_list.insert( list[index].vertex_list[n].adjacent_list.begin() + i, new_list);
              return n;
            }
          }
          list[index].vertex_list[n].adjacent_list.push_back(new_list);
          return n;
        }
      }
      VertexType vertex;
      vertex.send_id = send_id;
      vertex.adjacent_list.push_back(new_list);
      list[index].vertex_list.push_back(vertex);
      return list[index].vertex_list.size() - 1;
    }
  }
  // 這個函式是在已發送者學號在列表中的位置情況下可以直接找到並更新它的Adjacent_List
  void UpdateVertexAdjacentList(int index, int list_position, ListType new_list) {
    for (int i = 0; i < list[index].vertex_list[list_position].adjacent_list.size(); i++) {
      if (new_list.receive_id < list[index].vertex_list[list_position].adjacent_list[i].receive_id) {
        list[index].vertex_list[list_position].adjacent_list.insert( list[index].vertex_list[list_position].adjacent_list.begin() + i, new_list);
        return;
      }
    }
    list[index].vertex_list[list_position].adjacent_list.push_back(new_list);
    return;
  }
};
// 設計相鄰串列的一些功能
class AdjacencyLists {
  // 外部可讀寫的變數
 public:
  std::vector<VertexType> list;  // 主陣列
  // 外部可使用的函式
 public:
  void BuildList(Hash hash) {
    bool done = false;
    for (int i = 0; i < 109; i++) {
      if (hash.list[i].vertex_list.empty()) {
        continue;
      }
      for (int j = 0; j < hash.list[i].vertex_list.size(); j++) {
        done = false;
        if (list.empty()) {
          list.push_back(hash.list[i].vertex_list[j]);
          done = true;
          continue;
        }
        for (int n = 0; n < list.size(); n++) {
          if (hash.list[i].vertex_list[j].send_id < list[n].send_id) {
            list.insert(list.begin() + n, hash.list[i].vertex_list[j]);
            done = true;
            break;
          }
        }
        if (!done) {
          list.push_back(hash.list[i].vertex_list[j]);
        }
      }
    }
  }
};

class ConnectionCounts {
 public:
  std::vector<VertexTypeConnect> list;
 public:
 // 算連通數
 
 // 接到對應的地方(選擇排序)

 // 寫檔
};

class ProgramPackage {
  // 限class內部可讀寫的變數
 private:
  std::vector<DataType> dataset;  // 資料集，儲存從Bin檔讀取出來的資料
  std::string file_number;        // 檔案序號，例如pairs401.bin 的 401
  AdjacencyLists adj_list;
  // 外部可讀寫的變數
 public:
  bool doneMissionOne = false;  // 確認是否執行過任務一的boolean值
  // 外部可使用的函式
 public:
  // 讀取指定Bin檔中的資料
  bool ReadBinFile() {
    std::ifstream file;
    std::string file_name, file_line;
    // 列印輸入提示及讀取檔案編號
    std::cout << "Input a file number ([0] Quit): ";
    std::cin >> file_number;
    // 輸出風格調整空行
    std::cout << "\n";
    // 如果輸入為0，跳離讀檔函式
    if (file_number == "0") {
      return false;
    }
    // 組成完整檔名
    file_name = "pairs" + file_number + ".bin";
    //  開啟檔案 // in(input) / out(output)
    file.open(file_name, std::ios::in | std::ios::binary);
    // 確認檔案是否存在，如果不存在:輸出錯誤，跳離讀檔函式，存在:繼續執行讀取檔案內容
    if (!file.is_open()) {
      // 輸出錯誤:檔案不存在
      std::cout << "### " << file_name << " does not exist!" << " ###"
                << "\n\n";
      return false;
    }
    // 讀取檔案內資料部分
    // 清除之前執行過的資料集
    dataset.clear();
    DataType data;
    // 每次從檔案中讀取一個struct的資料，存進dataset中，重複執行直到檔案中讀不到新資料
    while (file.read(reinterpret_cast<char *>(&data), sizeof(data))) {
      dataset.push_back(data);
    }
    // 關閉檔案
    file.close();
    return true;
  }
  // 寫入.adj檔
  void WriteAdjFile() {
    std::ofstream file;
    std::string file_name = "pairs" + file_number + ".adj";
    file.open(file_name);
    file << "<<< There are " << adj_list.list.size() << " IDs in total. >>>" << "\n";
    for (int i = 0; i < adj_list.list.size(); i++) {
      file << "[" << std::setw(3) << i + 1 << "] " << adj_list.list[i].send_id << ": " << "\n";
      int count = 1;
      for (ListType data : adj_list.list[i].adjacent_list) {
        file << "\t" << "(" << std::setw(2) << count << ") " << data.receive_id << ",   " << std::setw(4) << data.weight;
        if (count == 12) {
          file << "\n";
        }
        count++;
      }
      file << "\n";
    }
    file << "<<< There are " << dataset.size() << " nodes in total. >>>" << "\n";
    file.close();
    return;
  }

  void BuildAdjacencyLists() {
    Hash hash;
    std::string previous_send_id = "";
    int previos_index = 0;
    int previos_list_position = 0;
    adj_list.list.clear();
    for (DataType data : dataset) {
      ListType new_list;
      new_list.receive_id = std::string(data.receive_id.data());
      new_list.weight = data.weight;
      hash.CheckReceiveIDIsExist(new_list.receive_id);
      if (previous_send_id == std::string(data.send_id.data())) {
        hash.UpdateVertexAdjacentList(previos_index, previos_list_position, new_list);
      } else {
        previous_send_id = std::string(data.send_id.data());
        previos_list_position = hash.UpdateVertexAdjacentList(std::string(data.send_id.data()), new_list, previos_index);
      }
    }
    adj_list.BuildList(hash);
    WriteAdjFile();
    std::cout << "<<< There are " << adj_list.list.size()
              << " IDs in total. >>>" << "\n\n";
    std::cout << "<<< There are " << dataset.size() << " nodes in total. >>>"
              << "\n\n";
    doneMissionOne = true;
    return;
  }
  void ComputeConnectionCounts() {
    // new_list;
  }
};

class System {
 private:
  ProgramPackage program_package;

 public:
  void PrintUI() {
    std::cout << "**** Graph data manipulation *****" << std::endl
              << "* 0. QUIT                        *" << std::endl
              << "* 1. Build adjacency lists       *" << std::endl
              << "* 2. Compute connection counts   *" << std::endl
              << "**********************************" << std::endl;
    return;
  }
  // 讀取執行指令
  int ReadCommand() {
    std::string command;
    // 列印UI
    PrintUI();
    // 列印輸入提示及讀取執行指令
    std::cout << "Input a choice(0, 1, 2): ";
    std::cin >> command;
    // 根據執行指令執行不同動作
    if (command == "0") {
      // 輸出風格調整空行
      std::cout << "\n";
      return 0;
    } else if (command == "1" || command == "2") {
      CallProgram(std::stoi(command));
    } else {
      // 輸出風格調整空行
      std::cout << "\n";
      std::cout << "Command does not exist!" << "\n\n";
    }
    return 4;
  }
  // 執行指定程式
  void CallProgram(int command) {
    // 執行任務一
    if (command == 1) {
      // 確認讀檔是否順利，不順利則跳離
      if (!program_package.ReadBinFile()) {
        return;
      }
      // 讀檔順利會執行任務一
      program_package.BuildAdjacencyLists();
    } else if (command == 2) {  // 執行任務二
      if (program_package.doneMissionOne == false) {
        std::cout << "### There is no graph and choose 1 first. ###" << "\n\n";
      }
      program_package.ComputeConnectionCounts();
    }
    return;
  }
};

int main() {
  System system;
  while (true) {
    if (system.ReadCommand() == 0) {
      break;
    }
  }
  return 0;
}