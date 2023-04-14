#include <vector>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstring>

#include <qpdf/QPDF.hh>
#include <qpdf/QPDFWriter.hh>

#ifdef _WIN32
	#define PATH_SEPARATOR "\\"
#else
	#define PATH_SEPARATOR "/"
#endif

static constexpr char BOM_MAGIC_NUMBERS[] = {0xEF, 0xBB, 0xBF};
static constexpr char PDF_MAGIC_NUMBERS[] = {0x25, 0x50, 0x44, 0x46};

static bool magic_matches(const std::string filename) {
	
	std::ifstream stream = std::ifstream(filename, std::ifstream::binary);
	
	if (!stream.is_open()) {
		return 0;
	}
	
	char buffer[7] = {'\0'};
	stream.read(buffer, sizeof(buffer));
	
	stream.close();
	
	const char* start = buffer;
	
	const bool bom = std::memcmp(start, BOM_MAGIC_NUMBERS, sizeof(BOM_MAGIC_NUMBERS)) == 0;
	
	if (bom) {
		start += sizeof(BOM_MAGIC_NUMBERS);
	}
	
	const bool matches = std::memcmp(buffer, PDF_MAGIC_NUMBERS, sizeof(PDF_MAGIC_NUMBERS)) == 0;
	
	return matches;
	
}

static void walk_directory(const std::string directory, const char* const pattern) {
	
	for (const std::filesystem::directory_entry entry : std::filesystem::directory_iterator(directory)) {
		const std::string name = entry.path().filename();
		
		if (entry.is_regular_file()) {
			std::cout << "+ Verificando o estado do arquivo presente em '" << name << "'" << std::endl;
			
			const bool matches = magic_matches(name);
			
			if (!matches) {
				std::cerr << "+ O arquivo localizado em '" << name << "' não é um documento PDF válido!" << std::endl;
				continue;
			}
			
			std::cout << "+ Os números mágicos do arquivo presente em '" << name << "' correspondem com os de um documento PDF; abrindo-o" << std::endl;
			
			const char* const path = name.c_str();
			
			size_t total_pages = 0;
			
			QPDF pdf;
			pdf.processFile(path, nullptr);
			
			QPDFObjectHandle null = QPDFObjectHandle::newNull();
			
			const std::vector<QPDFObjectHandle> pages = pdf.getAllPages();
			
			for (QPDFObjectHandle page : pages) {
				QPDFObjectHandle contents = page.getKey("/Contents");
				
				std::shared_ptr<Buffer> stream = contents.getStreamData();
				char* const buffer = (char*) stream->getBuffer();
				
				char* match = strstr(buffer, pattern);
				
				if (match != NULL) {
					char* start = match;
					
					while (!(start == match || *start == '\n' || *start == '\r')) {
						start--;
					}
					
					*start = '\0';
					contents.replaceStreamData(buffer, null, null);
					
					total_pages += 1;
				}
			}
			
			if (total_pages > 0) {
				std::string temporary_file = std::filesystem::temp_directory_path();
				
				temporary_file.append(PATH_SEPARATOR);
				temporary_file.append("document.pdf");
				
				std::cout << "+ Um total de " << total_pages << " páginas foram modificadas; exportando documento modificado para '" << temporary_file << "'" << std::endl;
				
				QPDFWriter writer = QPDFWriter(pdf, temporary_file.c_str());
				writer.setObjectStreamMode(qpdf_o_generate);
				writer.write();
				
				std::cout << "+ Movendo documento PDF de '" << temporary_file << "' para '" << name << "'" << std::endl;
				
				try {
					std::filesystem::rename(temporary_file, name);
				} catch (std::filesystem::filesystem_error) {
					std::filesystem::remove(name);
					std::filesystem::copy(temporary_file, name);
					std::filesystem::remove(temporary_file);
				}
			} else {
				std::cerr << "- Nenhuma das páginas do documento PDF presente em '" << name << "' corresponderam com o texto inserido!" << std::endl; 
			}
		} else if (entry.is_directory()) {
			walk_directory(name, pattern);
		}
		
	}
	
}

int main() {
	
	std::cout << "> Insira o texto a ser removido: ";
	
	std::string pattern;
	std::getline(std::cin, pattern);
	
	const std::string cwd = std::filesystem::current_path();
	const char* const cpattern = pattern.c_str();
	
	walk_directory(cwd, cpattern);
	
	return EXIT_SUCCESS;
	
}