
#include <unordered_map>
#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>

struct page_table_info_t {
	long long unsigned int VA;
	long long unsigned int PE1;
	long long unsigned int PE2;
	long long unsigned int PE3;
	long long unsigned int PE4;
	long long unsigned int PA;

	std::vector<long long unsigned int *> all;

	page_table_info_t() {
	this->PE1 = 0;
	this->PE2 = 0;
	this->PE3 = 0;
	this->PE4 = 0;

	this->all.resize(5);
	this->all[1] = &(this->PE1);
	this->all[2] = &(this->PE2);
	this->all[3] = &(this->PE3);
	this->all[4] = &(this->PE4);
	this->all[0] = &(this->PA );
	}

	page_table_info_t(const page_table_info_t &obj) {
	this->PE1 = obj.PE1;
	this->PE2 = obj.PE2;
	this->PE3 = obj.PE3;
	this->PE4 = obj.PE4;
	this->PA  = obj.PA;

	this->all.resize(5);
	this->all[1] = &(this->PE1);
	this->all[2] = &(this->PE2);
	this->all[3] = &(this->PE3);
	this->all[4] = &(this->PE4);
	this->all[0] = &(this->PA );
	}

};

typedef std::unordered_map<long long unsigned int, page_table_info_t> page_table_t;

int main() {
	page_table_t host_page_table;
	FILE* host_page_table_file = fopen("host.pt_dump2", "r");

	int host_page_table_record_num = 0;

	fscanf(host_page_table_file, "%d\n", &host_page_table_record_num);

	std::cout << "Loading host_page table with " <<  host_page_table_record_num << " PT entries...\n";

	for (int i = 0; i < host_page_table_record_num; i++) {
		page_table_info_t tmp;
		fscanf(host_page_table_file, "%llx,%llx,%llx,%llx,%llx,%llx\n", &(tmp.VA), &(tmp.PE1), &(tmp.PE2), &(tmp.PE3), &(tmp.PE4), &(tmp.PA));
		host_page_table.insert(std::make_pair(tmp.VA, tmp));
	}

	std::cout << "Loaded " << host_page_table.size() << " unique PT entries.\n";
	fclose(host_page_table_file);

	for(page_table_t::const_iterator it = host_page_table.begin(); it != host_page_table.end(); ++it)
	{
		page_table_info_t tmp = it->second;
		printf("%llx,%llx,%llx,%llx,%llx,%llx,%llx\n", (tmp.VA), it->first, (tmp.PE1), (tmp.PE2), (tmp.PE3), (tmp.PE4), (tmp.PA));
	}

	return 0;
}