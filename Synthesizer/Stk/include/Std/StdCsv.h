#pragma once

#include "Csv/rapidcsv.h"

namespace Std 
{
    struct SeparatorParams
    {
        rapidcsv::SeparatorParams params;

        SeparatorParams(const char pSeperator = ',', const bool pTrim = false,
                        const bool pHasCR = rapidcsv::sPlatformHasCR, const bool pQuotedLineBreaks = false,
                        const bool pAutoQuote = true)                             
        {
            params = rapidcsv::SeparatorParams(pSeperator,pTrim,pHasCR,pQuotedLineBreaks,pAutoQuote);
        }        
    };

    struct LabelParameters
    {
        rapidcsv::LabelParams params;
        
        LabelParameters(const int pColumnNameIdx = 0, const int pRowNameIdx = -1) {
            params = rapidcsv::LabelParams(pColumnNameIdx,pRowNameIdx);
        }
    };

    struct LineReaderParams 
    {
        rapidcsv::LineReaderParams params;

        LineReaderParams(const bool pSkipComment = false, const char pCommentPrefix = '#', const bool pSkipEmptyLines= false) {
            params = rapidcsv::LineReaderParams(pSkipComment,pCommentPrefix,pSkipEmptyLines);
        }
    };

    struct StdCsvDocument 
    {
        rapidcsv::Document *doc;

        StdCsvDocument(const std::string& filename) {
            doc = new rapidcsv::Document(filename);
            assert(doc != NULL);
        }
        ~StdCsvDocument() {
            if(doc) delete doc;
        }

        void Load(const std::string& filename) {
            if(doc) delete doc;
            doc = new rapidcsv::Document(filename);
            assert(doc != NULL);
        }

        void Save(const std::string& filename) {
            if(doc) doc->Save(filename);
        }

        void Clear() { doc->Clear(); }

        ssize_t GetColIndex(const std::string& idx) { return doc->GetColumnIdx(idx); }
        
        
        std::vector<double> GetColValues(const size_t col) { return doc->GetColumn<double>(col); }
        std::vector<double> GetColValues(const std::string& col) { return doc->GetColumn<double>(col); }

        std::vector<std::string> GetColStrings(const size_t col) { return doc->GetColumn<std::string>(col); }
        std::vector<std::string> GetColStrings(const std::string& col) { return doc->GetColumn<std::string>(col); }

        void SetColValues(const size_t col, const std::vector<double> & v) { doc->SetColumn<double>(col,v); }
        void SetColValues(const std::string& col, const std::vector<double> & v) { doc->SetColumn<double>(col,v); }

        void SetColStrings(const size_t col, const std::vector<std::string> & v) { doc->SetColumn<std::string>(col,v); }
        void SetColStrings(const std::string& col, const std::vector<std::string> & v) { doc->SetColumn<std::string>(col,v); }

        void RemoveCol(const size_t col) { doc->RemoveColumn(col); }
        void RemoveCol(const std::string& col) { doc->RemoveColumn(col); }

        void InsertCol(const size_t col, const std::vector<double> & v, const std::string& name = "") { doc->InsertColumn<double>(col,v,name); }

        size_t GetColCount() const { return doc->GetColumnCount(); }

        ssize_t GetRowIndex(const std::string& name) { return doc->GetRowIdx(name); }
        

        std::vector<double> GetRowValues(const size_t row) { return doc->GetRow<double>(row); }
        std::vector<double> GetRowValues(const std::string& row) { return doc->GetRow<double>(row); }

        std::vector<std::string> GetRowStrings(const size_t row) { return doc->GetRow<std::string>(row); }
        std::vector<std::string> GetRowStrings(const std::string& row) { return doc->GetRow<std::string>(row); }

        void SetRow(const size_t row, const std::vector<double> & v) { doc->SetColumn<double>(row,v); }
        void SetRow(const std::string& row, const std::vector<double> & v) { doc->SetColumn<double>(row,v); }

        void RemoveRow(const size_t row) { doc->RemoveRow(row); }
        void RemoveRow(const std::string& row) { doc->RemoveRow(row); }

        void InsertRow(const size_t row, const std::vector<double> & v, const std::string& name = "") { doc->InsertRow<double>(row,v,name); }

        size_t GetRowCount() const { return doc->GetRowCount(); }

        double GetCellValue(const size_t pColumnIdx, const size_t pRowIdx) { return doc->GetCell<double>(pColumnIdx,pRowIdx); }
        double GetCellValue(const std::string pColumnIdx, const std::string pRowIdx) { return doc->GetCell<double>(pColumnIdx,pRowIdx); }

        std::string GetCellString(const size_t pColumnIdx, const size_t pRowIdx) { return doc->GetCell<std::string>(pColumnIdx,pRowIdx); }
        std::string GetCellString(const std::string pColumnIdx, const std::string pRowIdx) { return doc->GetCell<std::string>(pColumnIdx,pRowIdx); }

        void SetCellValue(const size_t pColumnIdx, const size_t pRowIdx, double value) { return doc->SetCell<double>(pColumnIdx,pRowIdx,value); }
        void SetCellValue(const size_t pColumnIdx, const size_t pRowIdx, std::string& value) { return doc->SetCell<std::string>(pColumnIdx,pRowIdx,value); }

        void SetColName(const size_t col, const std::string& name) { doc->SetColumnName(col,name); }
        void SetRowName(const size_t row, const std::string& name) { doc->SetRowName(row,name); }

        std::string GetColName(const ssize_t idx) { return doc->GetColumnName(idx); }
        std::string GetRowName(const ssize_t idx) { return doc->GetRowName(idx); }
    };
}