// 資訊二乙 11227220張季平、11227235吳君友
#include <array>
#include <fstream>
#include <iomanip>  // 控制輸出至小數點後第幾位
#include <utility>  // 取用pair
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <queue>
#include <set>
#include <unordered_map>
// Bin檔中的資料型態
struct DataType {
  std::array<char, 12> send_id{};     // 發訊者學號
  std::array<char, 12> receive_id{};  // 收訊者學號
  float weight;                       // 量化權重
};

// 哈希表的資料型態
struct HashType {
  // key為發送者訊號，Value為相鄰點的Map
  std::map<std::string, std::map<std::string, float>> vertex_list;
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
    // 尋找收訊者學號在列表中的位置
    std::map<std::string, std::map<std::string, float>>::iterator position;
    position = list[index].vertex_list.find(id);
    // 如果位置等於.end()，代表沒找到
    // map.end()是一個虛擬的位置，不指向任何有效元素，只是用來表示「已超出範圍」
    if (position == list[index].vertex_list.end()) {
      list[index].vertex_list.insert(std::pair<std::string, std::map<std::string, float>>(id, {}));
    }
  }
  // 這個函式是要用來找到發送者學號在列表中位置並更新它的相鄰點列表
  void UpdateVertexAdjacentList(std::string send_id, std::pair<std::string, float>& new_recipient, int& index) {
    // 計算索引值
    index = CalculateHashValue(send_id);
    // 尋找發訊者學號在列表中的位置
    std::map<std::string, std::map<std::string, float>>::iterator position;
    position = list[index].vertex_list.find(send_id);
    // 如果位置等於.end()，代表沒找到
    // map.end()是一個虛擬的位置，不指向任何有效元素，只是用來表示「已超出範圍」
    if (position == list[index].vertex_list.end()) {
      list[index].vertex_list.insert(std::pair<std::string, std::map<std::string, float>>(send_id, {new_recipient}));
      return;
    }
    // 有找到的情況
    position -> second.insert(new_recipient);
    return;
  }
  // 這個函式是在已發送者學號在列表中的位置情況下可以直接找到並更新它的adjacent_vertex
  void UpdateVertexAdjacentList(int index, std::string send_id, std::pair<std::string, float>& new_recipient) {
    std::map<std::string, std::map<std::string, float>>::iterator position;
    position = list[index].vertex_list.find(send_id);
    position -> second.insert(new_recipient);
    return;
  }
};

// 設計相鄰串列的一些功能
class AdjacencyLists {
 // 外部可讀寫的變數
 public:
  // key為發送者訊號，Value為相鄰點的Map
  std::map<std::string, std::map<std::string, float>> list; // 主陣列
  std::map<int, std::map<std::string, std::set<std::string>>> connectionlist; // 連通點陣列
  std::map<int, std::map<std::string, std::set<std::string>>> influencelist; // 有效點陣列
 // 外部可使用的函式
 public:
  void BuildList(Hash hash) {
    for (int index = 0; index < 109; index++) {
      // 如果索引值對應的頂點陣列內沒有值，直接執行下一個索引值的檢索
      if (hash.list[index].vertex_list.empty()) {
        continue;
      }
      // 如果索引值對應的頂點陣列內有值，新增到清單中
      for (std::pair<std::string, std::map<std::string, float>> data : hash.list[index].vertex_list) {
        list.insert(data);
      }
    }
  }
  int CalculateConnectionNumber() {
    // 已完成的檢索表
    std::unordered_map<std::string, std::set<std::string>> visited;
    std::unordered_map<std::string, std::set<std::string>>::iterator position;
    std::map<std::string, std::set<std::string>> temp;
    // 接收者佇列
    std::queue<std::string> recipient;
    // 連接點列表
    std::set<std::string> junction;
    std::string vertex;
    int total_id = 0;
    // 以每個發訊者學號為Root來做一遍
    for (const auto& root : list) {
      recipient.push(root.first);
      while(!recipient.empty()) {
        // 取得最佇列最前面的學號
        vertex = recipient.front();
        recipient.pop();
        // // 如果vertex已經在junction裡面，直接pop掉
        // if (junction.count(vertex) == 1) {
        //   continue;
        // }
        // 查找vertex在不在visited裡面
        position = visited.find(vertex);
        // 代表曾經已經算過了
        if (position != visited.end()) {
          junction.emplace(vertex);
          for(const auto& connection_vertex : position -> second) {
            if (junction.count(connection_vertex) == 1 || connection_vertex == root.first) {
              continue;
            } else {
              junction.emplace(connection_vertex);
            }
          }
        } else { // 代表還沒算過
          for (const auto& new_recipient : list[vertex]) {
            if (junction.count(new_recipient.first) == 1 || new_recipient.first == root.first) {
              continue;
            }
            recipient.push(new_recipient.first);
            junction.emplace(new_recipient.first);
          }
        }
      }
      visited[root.first] = junction;
      junction.clear();
      total_id++;
    }
    for (const auto& data : visited) {
      if (connectionlist.count(data.second.size()) == 0) {
        temp.insert(std::pair<std::string, std::set<std::string>>(data.first, data.second));
        connectionlist.insert(std::pair<int, std::map<std::string, std::set<std::string>>>(data.second.size(), {temp}));
        temp.clear();
      } else {
        connectionlist[data.second.size()].insert(std::pair<std::string, std::set<std::string>>(data.first, data.second));
      }
    }
    return total_id;
  }
  int FindInfluencePath(float& real_number) {
    // 已完成的檢索表
    std::unordered_map<std::string, std::set<std::string>> visited;
    std::unordered_map<std::string, std::set<std::string>>::iterator position;
    std::map<std::string, std::set<std::string>> temp;
    // 接收者堆疊
    std::stack<std::string> recipient;
    // 連接點列表
    std::set<std::string> junction;
    std::string vertex;
    int total_id = 0;
    // 以每個發訊者學號為Root來做一遍
    for (const auto& root : list) {
      recipient.push(root.first);
      while(!recipient.empty()) {
        // 取得最佇列最前面的學號
        vertex = recipient.top();
        recipient.pop();
        // 查找vertex在不在visited裡面
        position = visited.find(vertex);
        // 代表曾經已經算過了
        if (position != visited.end()) {
          junction.emplace(vertex);
          for(const auto& connection_vertex : position -> second) {
            if (junction.count(connection_vertex) == 1 || connection_vertex == root.first) {
              continue;
            } else {
              junction.emplace(connection_vertex);
            }
          }
        } else { // 代表還沒算過
          for (const auto& new_recipient : list[vertex]) {
            if (junction.count(new_recipient.first) == 1 || new_recipient.first == root.first) {
              continue;
            }
            if (new_recipient.second < real_number) {
              continue;
            }
            recipient.push(new_recipient.first);
            junction.emplace(new_recipient.first);
          }
        }
      }
      visited[root.first] = junction;
      // std::cout << visited[root.first].size() << " ";
      junction.clear();
      total_id++;
    }
    for (const auto& data : visited) {
      if (data.second.size() == 0) {
        total_id--;
        continue;
      }
      if (influencelist.count(data.second.size()) == 0) {
        temp.insert(std::pair<std::string, std::set<std::string>>(data.first, data.second));
        influencelist.insert(std::pair<int, std::map<std::string, std::set<std::string>>>(data.second.size(), {temp}));
        temp.clear();
      } else {
        influencelist[data.second.size()].insert(std::pair<std::string, std::set<std::string>>(data.first, data.second));
      }
    }
    return total_id;
  }
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
    doneMissionOne = false;
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
    // 組合完整檔名
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
    // 組合完整檔名
    std::string file_name = "pairs" + file_number + ".adj";
    int count = 0;
    //  開啟檔案
    file.open(file_name);
    file << "<<< There are " << adj_list.list.size() << " IDs in total. >>>" << "\n";
    for (std::pair<std::string, std::map<std::string, float>> position : adj_list.list) {
      file << "[" << std::setw(3) << count + 1 << "] " << position.first << ": " << "\n";
      int receive_count = 1;
      for (std::pair<std::string, float> data : position.second) {
        file << "\t" << "(" << std::setw(2) << receive_count << ") " << data.first << ",   " << std::setw(4) << data.second;
        if (receive_count % 12 == 0) {
          file << "\n";
        }
        receive_count++;
      }
      file << "\n";
      count++;
    }
    file << "<<< There are " << dataset.size() << " nodes in total. >>>" << "\n";
    //  關閉檔案
    file.close();
    return;
  }
    // 寫入.cnt檔
  void WriteCntFile(int total_count) {
    std::ofstream file;
    // 組合完整檔名
    std::string file_name = "pairs" + file_number + ".cnt";
    int count = 0;
    //  開啟檔案
    file.open(file_name);
    file << "<<< There are " << total_count << " IDs in total. >>>";
    for (auto position = adj_list.connectionlist.rbegin(); position != adj_list.connectionlist.rend(); position++) {
      for (std::pair<std::string, std::set<std::string>> data : position -> second) {
        file << "\n" << "[" << std::setw(3) << count + 1 << "] " << data.first << "(" << position -> first << ")" << ": " << "\n";
        int connection_count = 1;
        for (std::string connection : data.second) {
          file << "\t" << "(" << std::setw(2) << connection_count << ") " << connection;
          if (connection_count % 12 == 0) {
            file << "\n";
          }
          connection_count++;
        }
        count++;
      }
    }
    file << "\n";
    //  關閉檔案
    file.close();
    return;
  }
  void WriteInfFile(int total_count) {
    std::ofstream file;
    // 組合完整檔名
    std::string file_name = "pairs" + file_number + ".inf";
    int count = 0;
    //  開啟檔案
    file.open(file_name);
    file << "<<< There are " << total_count << " IDs in total. >>>";
    for (auto position = adj_list.influencelist.rbegin(); position != adj_list.influencelist.rend(); position++) {
      for (std::pair<std::string, std::set<std::string>> data : position -> second) {
        file << "\n" << "[" << std::setw(3) << count + 1 << "] " << data.first << "(" << position -> first << ")" << ": " << "\n";
        int connection_count = 1;
        for (std::string connection : data.second) {
          file << "\t" << "(" << std::setw(2) << connection_count << ") " << connection;
          if (connection_count % 12 == 0) {
            file << "\n";
          }
          connection_count++;
        }
        count++;
      }
    }
    file << "\n";
    //  關閉檔案
    file.close();
    return;
  }
  // 建立相鄰串列
  void BuildAdjacencyLists() {
    Hash hash;
    std::string previous_send_id = "";
    int previos_index = 0;
    // 清除之前的紀錄
    adj_list.list.clear();
    for (DataType data : dataset) {
      std::pair<std::string, float> new_recipient(std::string(data.receive_id.data()), data.weight);
      // 檢查收訊者是否被錄入發訊者清單，沒有的話錄入
      hash.CheckReceiveIDIsExist(new_recipient.first);
      // 如果這次發訊者跟上次一樣，直接進入指定位置添加關聯點
      if (previous_send_id == std::string(data.send_id.data())) {
        hash.UpdateVertexAdjacentList(previos_index, previous_send_id, new_recipient);
      } else {
        // 如果這次發訊者跟上次不一樣，進入函式添加關聯點
        previous_send_id = std::string(data.send_id.data());
        hash.UpdateVertexAdjacentList(std::string(data.send_id.data()), new_recipient, previos_index);
      }
    }
    // 統整進關聯陣列中
    adj_list.BuildList(hash);
    // 寫檔
    WriteAdjFile();
    // 輸出訊息
    std::cout << "<<< There are " << adj_list.list.size()
              << " IDs in total. >>>" << "\n\n";
    std::cout << "<<< There are " << dataset.size() << " nodes in total. >>>"
              << "\n\n";
    // 執行完任務一的訊號
    doneMissionOne = true;
    return;
  }
  void ComputeConnectionCounts() {
    int total_id = 0;
    adj_list.connectionlist.clear();
    // new_list;
    total_id = adj_list.CalculateConnectionNumber();
    WriteCntFile(total_id);
    std::cout << "\n" << "<<< There are " << total_id << " IDs in total. >>>" << "\n\n";
  }
  bool CheckIsFloat(std::string& str) {
    for (int i = 0; i < str.length(); i++) {
      if (!std::isdigit(str[i]) && str[i] != '.') {
        return false;
      }
    }
    return true;
  }
  void EstimateInfluenceValues() {
    std::string real_string;
    float real_number = 0.0;
    while (true) {
      std::cout << "\n" << "Input a real number in [0,1]: ";
      std::cin >> real_string;
      if (!CheckIsFloat(real_string)) {
        continue;
      }
      real_number = std::stof(real_string);
      if (1 >= real_number && real_number >= 0) {
        std::cout << "\n";
        break;
      }
      std::cout << "\n" << "### " << real_number << " is NOT in [0,1] ###" << "\n";
    }
    int total_id = adj_list.FindInfluencePath(real_number);
    WriteInfFile(total_id);
    std::cout << "<<< There are " << total_id << " IDs in total. >>>" << "\n\n";
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
              << "* 3. Estimate influence values   *" << std::endl
              << "**********************************" << std::endl;
    return;
  }
  // 讀取執行指令
  int ReadCommand() {
    std::string command;
    // 列印UI
    PrintUI();
    // 列印輸入提示及讀取執行指令
    std::cout << "Input a choice(0, 1, 2, 3): ";
    std::cin >> command;
    // 根據執行指令執行不同動作
    if (command == "0") {
      // 輸出風格調整空行
      std::cout << "\n";
      return 0;
    } else if (command == "1" || command == "2" || command == "3") {
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
      std::cout << "\n";
      // 確認讀檔是否順利，不順利則跳離
      if (!program_package.ReadBinFile()) {
        return;
      }
      // 讀檔順利會執行任務一
      program_package.BuildAdjacencyLists();
    } else if (command == 2) {  // 執行任務二
      if (program_package.doneMissionOne == false) {
        std::cout << "### There is no graph and choose 1 first. ###" << "\n\n";
        return;
      }
      program_package.ComputeConnectionCounts();
    } else if (command == 3) {
      if (program_package.doneMissionOne == false) {
        std::cout << "### There is no graph and choose 1 first. ###" << "\n\n";
        return;
      }
      program_package.EstimateInfluenceValues();
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