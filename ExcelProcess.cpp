#include "stdafx.h"
#include "ExcelProcess.h"
#include "..\Header\CommonHeader.h"

//Ϊ�����ڴ����жദ��ExcelProcess����new����ExcelProcess���Ϊ����ģʽ
//ͨ��getInstance��destroyInstance��ȡ�����ٶ���
ExcelProcess* ExcelProcess::excel = nullptr;

ExcelProcess::ExcelProcess()
{
	lpDisp = NULL;
	excelVer = 0;
	//init();
}

//��ȡ����
ExcelProcess* ExcelProcess::getInstance()
{
	if(excel == NULL)
	{
		excel = new ExcelProcess();
	}
	return excel;
}
//���ٶ���
void ExcelProcess::destroyInstance()
{
	if(excel != NULL)
	{
		delete excel;
		excel = NULL;
	}
}

ExcelProcess::~ExcelProcess()
{
	try
	{
		sheet.ReleaseDispatch();
		sheets.ReleaseDispatch();
		book.ReleaseDispatch();
		books.ReleaseDispatch();
		ExcelApp.ReleaseDispatch();
		ExcelApp.Quit();
		//�˳�αװ��app
		if(!ExcelApp_fake.get_ActiveSheet())
		{
			books_fake.ReleaseDispatch();
			ExcelApp_fake.ReleaseDispatch();
			ExcelApp_fake.Quit();
		}
	}
	catch (COleDispatchException*)
	{
		AfxMessageBox(Notice_get_by_id(IDS_POW_OFF_EXCEL_FAIL));
		//AfxMessageBox(_T("�ر�Excel�������"));
	}
}


/************************************************************************/
/* ��ʼ��                                                               */
/************************************************************************/
BOOL ExcelProcess::init()
{
	CString strOfficeVer[5] = {_T("office 2003"),_T("office 2007"),_T("office 2010"),_T("office 2013"), _T("office 2016")};
	BOOL result = FALSE;
	for(int i = 4; i >= 0; i--)
	{
		if(!createServer(strOfficeVer[i]))
			continue;
		else
		{
			result = TRUE;
		}
	}
	if(excelVer == 0)
	{
		result = FALSE;
	}
	return result;
}

//��ȡoffice�汾
int ExcelProcess::getOfficeVer()
{
	return excelVer;
}

/************************************************************************/
/* ����αExcel����
/* ��Ϊ��Щoffice�汾(��2007)�Ļ�����һ��EXE���̶�Ӧ���Excel�ļ�
/* ���������αExcel���񣬵��û�������excel�ļ�ʱ�����ԭ����excel�ļ���������
/* ��αExcel��������ṩ���û���������excel�ļ�(���ܷ���)
/************************************************************************/
BOOL ExcelProcess::createFakeServer()
{
	CString strOfficeVer[5] = {_T("office 2003"),_T("office 2007"),_T("office 2010"),_T("office 2013"), _T("office 2016")};
	for(int i = 4; i >= 0; i--)
	{
		CString officeVer = strOfficeVer[i];
		//ȥ��ǰ��ո�
		officeVer.Trim();
		//��ȡ�汾���ַ�
		CString verNum = officeVer.Right(4);
		int ver = _ttoi(verNum);
		switch(ver)
		{
		case 2003:
			if(judgeExcelVer(11))
			{
				if(ExcelApp_fake.CreateDispatch(_T("Excel.Application.11"), NULL))
				{
					ExcelApp_fake.put_DisplayAlerts(FALSE);
					books_fake.AttachDispatch(ExcelApp_fake.get_Workbooks());
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			break;
		case 2007:
			if(judgeExcelVer(12))
			{
				if(ExcelApp_fake.CreateDispatch(_T("Excel.Application.12"), NULL))
				{
					ExcelApp_fake.put_DisplayAlerts(FALSE);
					books_fake.AttachDispatch(ExcelApp_fake.get_Workbooks());
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			break;
		case 2010:
			if(judgeExcelVer(14))
			{
				if(ExcelApp_fake.CreateDispatch(_T("Excel.Application.14"), NULL))
				{
					ExcelApp_fake.put_DisplayAlerts(FALSE);
					books_fake.AttachDispatch(ExcelApp_fake.get_Workbooks());
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			break;
		case 2013:
			if( judgeExcelVer(15))
			{
				if(ExcelApp_fake.CreateDispatch(_T("Excel.Application.15"), NULL))
				{
					ExcelApp_fake.put_DisplayAlerts(FALSE);
					books_fake.AttachDispatch(ExcelApp_fake.get_Workbooks());
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			break;
		case 2016:
			if( judgeExcelVer(16))
			{
				if(ExcelApp_fake.CreateDispatch(_T("Excel.Application.16"), NULL))
				{
					ExcelApp_fake.put_DisplayAlerts(FALSE);
					books_fake.AttachDispatch(ExcelApp_fake.get_Workbooks());
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			break;
		}
	}
	return FALSE;
}

/************************************************************************/
/* �ж�Excel�汾��                                                      */
/************************************************************************/
BOOL ExcelProcess::judgeExcelVer(int Ver)
{
	HKEY hkey;
	int ret;
	CString str;	
	LONG len;
	str.Format(_T("Excel.Application.%d"),Ver);
	str += _T("\\CLSID");
	ret = RegCreateKey(HKEY_CLASSES_ROOT, str, &hkey);
	if(ret == ERROR_SUCCESS)
	{
		RegQueryValue(HKEY_CLASSES_ROOT, str, NULL, &len);
		//���ע����� HKEY_CLASSES_ROOT\Excel.Application.x\CPLSID�е�ֵΪ�գ����ȡ��'\0'������Ϊ2
		return len == 2 ? FALSE : TRUE;
	}
	else
	{
		return FALSE;
	}
}

/************************************************************************/
/* ����Excel���񣬴�����ַ���������ʽΪ office ****                    */
/************************************************************************/
BOOL ExcelProcess::createServer( CString officeVer )
{
	//ȥ��ǰ��ո�
	officeVer.Trim();
	//��ȡ�汾���ַ�
	CString verNum = officeVer.Right(4);
	int ver = _ttoi(verNum);
	switch(ver)
	{
	case 2003:
		if(judgeExcelVer(11))
		{
			if(ExcelApp.CreateDispatch(_T("Excel.Application.11"), NULL))
			{
				excelVer = 2003;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case 2007:
		if(judgeExcelVer(12))
		{
			if(ExcelApp.CreateDispatch(_T("Excel.Application.12"), NULL))
			{
				excelVer = 2007;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case 2010:
		if(judgeExcelVer(14))
		{
			if(ExcelApp.CreateDispatch(_T("Excel.Application.14"), NULL))
			{
				excelVer = 2010;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case 2013:
		if( judgeExcelVer(15))
		{
			if(ExcelApp.CreateDispatch(_T("Excel.Application.15"), NULL))
			{
				excelVer = 2013;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case 2016:
		if( judgeExcelVer(16))
		{
			if(ExcelApp.CreateDispatch(_T("Excel.Application.16"), NULL))
			{
				excelVer = 2016;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
		break;
	}
	return TRUE;
}

BOOL ExcelProcess::setExcelExportSheets( CString filePath )
{
// 	ExcelApp.put_Visible(TRUE);
// 	ExcelApp.put_UserControl(FALSE);
	//��ȡ���������� 
	books.AttachDispatch(ExcelApp.get_Workbooks());
	//��һ��������
	try
	{
		lpDisp = books.Open(filePath, 
			vtMissing, vtMissing, vtMissing, vtMissing, vtMissing,
			vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, 
			vtMissing, vtMissing, vtMissing, vtMissing);
		book.AttachDispatch(lpDisp);
	}
	catch(...)
	{
		//�½�
		lpDisp = books.Add(vtMissing);
		book.AttachDispatch(lpDisp);
		saveExcelAs(filePath);
	}

	sheets.AttachDispatch(book.get_Sheets());
	return TRUE;
}

//��ʾexcel�ļ�����������������������ʹ��
void ExcelProcess::setView()
{
	ExcelApp.put_Visible(TRUE);
	ExcelApp.put_UserControl(FALSE);
}

BOOL ExcelProcess::openExcelFile( CString filePath )
{
	return setExcelImportSheets(filePath);
}

BOOL ExcelProcess::setExcelImportSheets( CString filePath )
{
	if (filePath.GetLength() == 0)
		return FALSE;
	// 	ExcelApp.put_Visible(TRUE);
	// 	ExcelApp.put_UserControl(FALSE);
	//��ȡ���������� 
	books.AttachDispatch(ExcelApp.get_Workbooks());

	//��һ��������
	try
	{
		lpDisp = books.Open(filePath, 
			vtMissing, vtMissing, vtMissing, vtMissing, vtMissing,
			vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, 
			vtMissing, vtMissing, vtMissing, vtMissing);
		book.AttachDispatch(lpDisp);
	}
	catch(...)
	{
		return FALSE;
	}
	//��ȡ�������� 
	sheets.AttachDispatch(book.get_Worksheets());

	// 	//�õ���ǰ��Ծsheet
	// 	//����е�Ԫ�������ڱ༭״̬�У��˲������ܷ��أ���һֱ�ȴ�
	// 	lpDisp=book.get_ActiveSheet();
	// 	sheet.AttachDispatch(lpDisp); 
	return TRUE;
}

//��ȡһ��ָ�����Ƶ�sheet����������򷵻�false��������ѡ�и�sheet
BOOL ExcelProcess::getSheet( CString sheetName )
{
	sheets.AttachDispatch(book.get_Sheets());
	try
	{
		lpDisp = sheets.get_Item(_variant_t(sheetName));
		sheet.AttachDispatch(lpDisp);
	}
	catch(...)
	{
// 		lpDisp = sheets.Add(vtMissing, vtMissing, _variant_t(1), vtMissing);
// 		sheet.AttachDispatch(lpDisp);
// 		sheet.put_Name(sheetName);
		return FALSE;
	}
	return TRUE;
}

//����sheet
void ExcelProcess::createSheet(CString sheetName)
{
	lpDisp = sheets.Add(vtMissing, vtMissing, _variant_t(1), vtMissing);
	sheet.AttachDispatch(lpDisp);
	sheet.put_Name(sheetName);
}

//��ȡ���ڵ�sheet(��ָ������)
BOOL ExcelProcess::getActiveSheet()
{
	//sheets.AttachDispatch(book.get_Sheets());
	try
	{
		lpDisp = book.get_ActiveSheet();
		sheet.AttachDispatch(lpDisp);
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}

//ɾ��sheet
BOOL ExcelProcess::deleteSheet( CString sheetName )
{
	try
	{
		//ɾ��ϵͳĬ�ϲ���������sheet
		sheet.AttachDispatch(sheets.get_Item(_variant_t(sheetName)));
		sheet.Delete();
	}
	catch(...)
	{
		//TODO
		return FALSE;
	}
	return TRUE;
}

/************************************************************************/
/* ���õ�Ԫ��������Ч��Ϊ����                                           */
/************************************************************************/
BOOL ExcelProcess::setCellsTypeToNum( CString cellBegin, CString cellEnd, int min, int max, int defValue )
{

	range = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	LPDISPATCH result = range.get_Validation();
	validation.AttachDispatch(result);
	validation.Delete();
	validation.Add(XlDVType::xlValidateWholeNumber, _variant_t(XlDVAlertStyle::xlValidAlertStop),
		_variant_t(XlFormatConditionOperator::xlBetween), _variant_t(min), _variant_t(max));
	range.put_Value2(_variant_t(defValue));
	setCellsAlignLeft(range);
	return TRUE;
}

//����
BOOL ExcelProcess::setCellsTypeToNum( CString cellBegin, CString cellEnd, int min, int max )
{
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	LPDISPATCH result = range.get_Validation();
	validation.AttachDispatch(result);
	validation.Delete();
	validation.Add(XlDVType::xlValidateWholeNumber, _variant_t(XlDVAlertStyle::xlValidAlertStop),
		_variant_t(XlFormatConditionOperator::xlBetween), _variant_t(min), _variant_t(max));
	setCellsAlignLeft(range);
	return TRUE;
}

//����
BOOL ExcelProcess::setCellsTypeToNum( CString cellBegin, CString cellEnd, double min, double max )
{
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	LPDISPATCH result = range.get_Validation();
	validation.AttachDispatch(result);
	validation.Delete();
	validation.Add(XlDVType::xlValidateDecimal, _variant_t(XlDVAlertStyle::xlValidAlertStop),
		_variant_t(XlFormatConditionOperator::xlBetween), _variant_t(min), _variant_t(max));
	setCellsAlignLeft(range);
	return TRUE;
}

/************************************************************************/
/* ���õ�Ԫ��������Ч��Ϊ�ַ�������                                     */
/************************************************************************/
BOOL ExcelProcess::setCellsToStringList( CString cellBegin, CString cellEnd, const std::vector<CString>& strList, unsigned int defValueIndex /*= 0*/ )
{
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	CString strDef = strList.at(defValueIndex);
	CString str;
	for(unsigned int i = 0; i < strList.size(); i++)
	{
		str += strList.at(i);
		str += ',';
	}
	range.put_Value2(_variant_t(strDef));
	LPDISPATCH result = range.get_Validation();
	validation.AttachDispatch(result);
	validation.Delete();
	validation.Add(XlDVType::xlValidateList, _variant_t(XlDVAlertStyle::xlValidAlertStop), _variant_t(XlFormatConditionOperator::xlBetween), _variant_t(str), _variant_t(NULL));
	setCellsAlignLeft(range);
	return TRUE;
}

//����
BOOL ExcelProcess::setCellsToStringList( CString cellBegin, CString cellEnd,const std::vector<CString>& strList )
{
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	CString str;
	for(unsigned int i = 0; i < strList.size(); i++)
	{
		str += strList.at(i);
		str += ',';
	}
	LPDISPATCH result = range.get_Validation();
	validation.AttachDispatch(result);
	validation.Delete();
	validation.Add(XlDVType::xlValidateList, _variant_t(XlDVAlertStyle::xlValidAlertStop), _variant_t(XlFormatConditionOperator::xlBetween), _variant_t(str), _variant_t(NULL));
	setCellsAlignLeft(range);
	return TRUE;
}

//����
BOOL ExcelProcess::setCellsToStringList(CString cellBegin, CString cellEnd, CString sheetName, CString valueCellBegin, CString valueCellEnd)
{
	valueCellBegin.Insert(0, _T("$"));
	valueCellEnd.Insert(0, _T("$"));
	int index = 0;
	for(index = 0; index < valueCellBegin.GetLength(); index++)
	{
		if(valueCellBegin.GetAt(index) >= _T('0') && valueCellBegin.GetAt(index) <= _T('9'))
			break;
	}
	valueCellBegin.Insert(index, _T("$"));
	index = 0;
	for(index = 0; index < valueCellEnd.GetLength(); index++)
	{
		if(valueCellEnd.GetAt(index) >= _T('0') && valueCellEnd.GetAt(index) <= _T('9'))
			break;
	}
	valueCellEnd.Insert(index, _T("$"));
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	CString str;
	str.Format(_T("=%s!%s:%s"), sheetName, valueCellBegin, valueCellEnd);

	LPDISPATCH result = range.get_Validation();
	validation.AttachDispatch(result);
	validation.Delete();
	validation.Add(XlDVType::xlValidateList, _variant_t(XlDVAlertStyle::xlValidAlertStop), _variant_t(XlFormatConditionOperator::xlBetween), _variant_t(str), _variant_t(NULL));
	setCellsAlignLeft(range);
	return TRUE;
}

//����
BOOL ExcelProcess::setCellsToStringList(CString cellBegin, CString cellEnd, const CString* strList, unsigned int len, unsigned int defValueIndex)
{
	if(strList == nullptr || defValueIndex >= len)
		return FALSE;
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	CString str;
	CString strDef = strList[defValueIndex];
	for(unsigned int i = 0; i < len; i++)
	{
		str += strList[i];
		str += ',';
	}
	range.put_Value2(_variant_t(strDef));
	LPDISPATCH result = range.get_Validation();
	validation.AttachDispatch(result);
	validation.Delete();
	validation.Add(XlDVType::xlValidateList, _variant_t(XlDVAlertStyle::xlValidAlertStop), _variant_t(XlFormatConditionOperator::xlBetween), _variant_t(str), _variant_t(NULL));
	setCellsAlignLeft(range);
	return TRUE;
}

//����
BOOL ExcelProcess::setCellsToStringList(CString cellBegin, CString cellEnd, const CString* strList, unsigned int len)
{
	if(strList == nullptr)
		return FALSE;
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	CString str;
	for(unsigned int i = 0; i < len; i++)
	{
		str += strList[i];
		str += ',';
	}
	LPDISPATCH result = range.get_Validation();
	validation.AttachDispatch(result);
	validation.Delete();
	validation.Add(XlDVType::xlValidateList, _variant_t(XlDVAlertStyle::xlValidAlertStop), _variant_t(XlFormatConditionOperator::xlBetween), _variant_t(str), _variant_t(NULL));
	setCellsAlignLeft(range);
	return TRUE;
}

//����Excel��ʽ���õ�Ԫ���������Ч��
void ExcelProcess::setCellsToFormula(CString cellBegin, CString cellEnd, CString formulaName)
{
	range = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	LPDISPATCH result = range.get_Validation();
	validation.AttachDispatch(result);
	validation.Delete();
	validation.Add(XlDVType::xlValidateList, _variant_t(XlDVAlertStyle::xlValidAlertStop), _variant_t(XlFormatConditionOperator::xlBetween),
		_variant_t(formulaName), vtMissing);
	setCellsAlignLeft(range);
}


/************************************************************************/
/* ���õ�Ԫ����ɫ����ɫ������ Excel��ɫ���ձ�.doc                       */
/************************************************************************/
BOOL ExcelProcess::setCellsColor(CString cellBegin, CString cellEnd, int colorIndex)
{
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	LPDISPATCH result = range.get_Interior();
	interior.AttachDispatch(result);
	interior.put_ColorIndex(_variant_t(colorIndex));
	return TRUE;
}

/************************************************************************/
/* ����һƬ��Ԫ���ֵ
/* cellBegin �������
/* ��Χ�ĳ�����nums����
/************************************************************************/
BOOL ExcelProcess::setCellsValue( CString cellBegin, vector<vector<CString>>& nums )
{
	COleSafeArray safeArr;
	//��һ������Ϊ���⣬�ӵ����п�ʼȥ��ȡ����
	//DWORD numElements[] = {nums.size(), nums.size() > 1 ? nums.at(nums.size() - 1).size() : nums.at(0).size()};
	DWORD numElements[] = {nums.size(), nums.at(0).size()};
	//������ά����
	safeArr.Create(VT_BSTR , 2, numElements);

	CString cellEnd = getEndCell(cellBegin, nums.size(), nums.at(0).size());
	range = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));

	long index[2] = {0, 0};
	long lFirstLBound = 0;
	long lFirstUBound = 0;
	long lSecondLBound = 0;
	long lSecondUBound = 0;
	//��ȡ�е�����
	safeArr.GetLBound(1, &lFirstLBound);
	//��ȡ�е�����
	safeArr.GetUBound(1, &lFirstUBound);
	//��ȡ�е�����
	safeArr.GetLBound(2, &lSecondLBound);
	//��ȡ�е�����
	safeArr.GetUBound(2, &lSecondUBound);
	//����
	for(ULONG i = lFirstLBound; i < nums.size(); i++)
	{
		index[0] = i;
		for(ULONG j = lSecondLBound; j < nums.at(i).size(); j++)
		{
			index[1] = j;
			//CString lElement = *((CString*)nums + cols*i + j);
			//CString lElement = nums[i][j];
			CString lElement = nums.at(i).at(j);
			BSTR strTmp = lElement.AllocSysString();
			safeArr.PutElement(index, strTmp);
			SysFreeString(strTmp);
		}
	}
	range.put_Value2((VARIANT) safeArr);
	setCellsAlignLeft(range);
	return TRUE;
}

//���أ���Ҫ�����������
BOOL ExcelProcess::setCellsValue( CString cellBegin, vector<int>& nums )
{
	COleSafeArray safeArr;
	DWORD numElements[] = {nums.size(), 1};
	//������ά����
	safeArr.Create(VT_I4 , 2, numElements);
	//ֻ��һ������
	CString cellEnd = getEndCell(cellBegin, nums.size(), 1);
	range = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));

	long index[2] = {0, 0};
	long lFirstLBound = 0;
	long lFirstUBound = 0;
	long lSecondLBound = 0;
	long lSecondUBound = 0;
	//��ȡ�е�����
	safeArr.GetLBound(1, &lFirstLBound);
	//��ȡ�е�����
	safeArr.GetUBound(1, &lFirstUBound);
	//��ȡ�е�����
	safeArr.GetLBound(2, &lSecondLBound);
	//��ȡ�е�����
	safeArr.GetUBound(2, &lSecondUBound);
	//����
	for(ULONG i = lFirstLBound; i < nums.size(); i++)
	{
		index[0] = i;
		index[1] = lSecondLBound;
		int val = nums.at(i);
		safeArr.PutElement(index, &val);
	}
	range.put_Value2((VARIANT) safeArr);
	setCellsAlignLeft(range);
	setCellsBold(cellBegin, cellEnd, TRUE);
	return TRUE;
}

//���� ��ʱ����
BOOL ExcelProcess::setCellsValue( CString cellBegin, long** nums, int rows, int cols )
{
	COleSafeArray safeArr;
	DWORD numElements[] = {rows, cols};
	//������ά����
	safeArr.Create(VT_I4 , 2, numElements);

	CString cellEnd = getEndCell(cellBegin, rows, cols);
	range = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));

	long index[2] = {0, 0};
	long lFirstLBound = 0;
	long lFirstUBound = 0;
	long lSecondLBound = 0;
	long lSecondUBound = 0;
	//��ȡ�е�����
	safeArr.GetLBound(1, &lFirstLBound);
	//��ȡ�е�����
	safeArr.GetUBound(1, &lFirstUBound);
	//��ȡ�е�����
	safeArr.GetLBound(2, &lSecondLBound);
	//��ȡ�е�����
	safeArr.GetUBound(2, &lSecondUBound);
	//����
	for(long i = lFirstLBound; i <= lFirstUBound; i++)
	{
		index[0] = i;
		for(long j = lSecondLBound; j <= lSecondUBound; j++)
		{
			index[1] = j;
			long lElement = *((long*)nums + cols*i + j);
			safeArr.PutElement(index, &lElement);
		}
	}
	/*VARIANT varWrite = (VARIANT) safeArr;*/
	range.put_Value2((VARIANT) safeArr);
	setCellsAlignLeft(range);
	return TRUE;
}

//���� ��ʱ����
BOOL ExcelProcess::setCellsValue( CString cellBegin, CString** nums, int rows, int cols )
{
	COleSafeArray safeArr;
	DWORD numElements[] = {rows, cols};
	//������ά����
	safeArr.Create(VT_BSTR , 2, numElements);

	CString cellEnd = getEndCell(cellBegin, rows, cols);
	range = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));

	long index[2] = {0, 0};
	long lFirstLBound = 0;
	long lFirstUBound = 0;
	long lSecondLBound = 0;
	long lSecondUBound = 0;
	//��ȡ�е�����
	safeArr.GetLBound(1, &lFirstLBound);
	//��ȡ�е�����
	safeArr.GetUBound(1, &lFirstUBound);
	//��ȡ�е�����
	safeArr.GetLBound(2, &lSecondLBound);
	//��ȡ�е�����
	safeArr.GetUBound(2, &lSecondUBound);
	//����
	for(long i = lFirstLBound; i <= lFirstUBound; i++)
	{
		index[0] = i;
		for(long j = lSecondLBound; j <= lSecondUBound; j++)
		{
			index[1] = j;
			//CString lElement = *((CString*)nums + cols*i + j);
			CString lElement = nums[i][j];
			BSTR strTmp = lElement.AllocSysString();
			safeArr.PutElement(index, strTmp);
			SysFreeString(strTmp);
		}
	}
	range.put_Value2((VARIANT) safeArr);
	setCellsAlignLeft(range);
	return TRUE;
}

//��ȡһ������ 
//cellBegin ������� 
//count     ����
void ExcelProcess::getColValue(vector<CString>& outData, CString cellBegin, int count)
{
	CString cellEnd = getEndCell(cellBegin, count, 1);
	range = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	VARIANT data = range.get_Value2();
	COleSafeArray safeArr(data);
	long index[2] = {0, 0};
	long lFirstLBound = 0;
	long lFirstUBound = 0;
	long lSecondLBound = 0;
	long lSecondUBound = 0;
	//��ȡ�е�����
	safeArr.GetLBound(1, &lFirstLBound);
	//��ȡ�е�����
	safeArr.GetUBound(1, &lFirstUBound);
	//��ȡ�е�����
	safeArr.GetLBound(2, &lSecondLBound);
	//��ȡ�е�����
	safeArr.GetUBound(2, &lSecondUBound);
	ASSERT(lSecondLBound == 1);
	ASSERT(lSecondUBound == 1);
	VARIANT value;
	for(long i = lFirstLBound; i <= lFirstUBound; i++)
	{
		index[0] = i;
		index[1] = 1;
		safeArr.GetElement(index, &value);
		CString valueStr;
		switch (value.vt)
		{
		case VT_BOOL:
			{
				BOOL t = value.boolVal;
				valueStr = t ? _T("TRUE") : _T("FALSE");
				break;
			}
		case VT_BSTR:
			{
				valueStr = value.bstrVal;
				break;
			}
		case VT_I4:
			{
				int t = value.intVal;
				valueStr.Format(_T("%d"), t);
				break;
			}
		case VT_R8:
			{
				double t = value.dblVal;
				valueStr.Format(_T("%lf"), t);
				break;
			}
		default:
			break;
		}
		outData.push_back(valueStr);
	}
}

//����һ������
BOOL ExcelProcess::setColValue( CString cellBegin, const vector<CString>& nums )
{
	COleSafeArray safeArr;
	DWORD numElements[] = {nums.size(), 1};
	//������ά����
	safeArr.Create(VT_BSTR , 2, numElements);
	CString cellEnd = getEndCell(cellBegin, nums.size(), 1);
	range = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	long index[2] = {0, 0};
	long lFirstLBound = 0;
	long lFirstUBound = 0;
	long lSecondLBound = 0;
	long lSecondUBound = 0;

	//��ȡ�е�����
	safeArr.GetLBound(1, &lFirstLBound);
	//��ȡ�е�����
	safeArr.GetUBound(1, &lFirstUBound);
	//��ȡ�е�����
	safeArr.GetLBound(2, &lSecondLBound);
	//��ȡ�е�����
	safeArr.GetUBound(2, &lSecondUBound);
	//����
	for(ULONG i = lFirstLBound; i < nums.size(); i++)
	{
		index[0] = i;
		index[1] = 0;
		CString lElement = nums.at(i);
		BSTR strTmp = lElement.AllocSysString();
		safeArr.PutElement(index, strTmp);
		SysFreeString(strTmp);
	}
	range.put_Value2((VARIANT) safeArr);
	//setCellsAlignLeft(range);
	return TRUE;
}


//���֧�ֿ�ͷ��������ĸ
//����������꣬���������������ؽ���������
//�紫�� A1,2,1 ���� A2  
//�紫�� A1,1,2 ���� B2 
CString ExcelProcess::getEndCell( CString cellBegin, int rows, int cols )
{
	BOOL hasTwoChar = FALSE;
	TCHAR beginChar = cellBegin[0];
	if(!((beginChar >= 'A' && beginChar <= 'Z')||(beginChar >= 'a' && beginChar <= 'z')))
	{
		//AfxMessageBox(_T("������ʼ�����ݴ���"));
		return CString("");
	}
	beginChar = cellBegin[1];
	{
		if(beginChar >= '0' && beginChar <= '9')
			beginChar = cellBegin[0];
		else
		{
			beginChar = cellBegin[1];
			hasTwoChar = TRUE;
			if(!((beginChar >= 'A' && beginChar <= 'Z')||(beginChar >= 'a' && beginChar <= 'z')))
			{
				//AfxMessageBox(_T("������ʼ�����ݴ���"));
				return CString("");
			}

		}
	}
	if(cols <= 0)
	{
		//AfxMessageBox(_T("�����������Ҫ����0"));
		return CString("");
	}
	CString numStr;
	if(!hasTwoChar)
		numStr = cellBegin.Mid(1, cellBegin.GetLength() - 1);
	else
		numStr = cellBegin.Mid(2, cellBegin.GetLength() - 1);
	int beginRow = _ttol(numStr);
	int endRow = beginRow + rows - 1;
	TCHAR endChar = beginChar + cols - 1;
	if(beginChar + cols - 1 > _T('Z'))
	{
		if(!hasTwoChar)
		{
			int index = (beginChar + cols - 1)% _T('Z');
			CString a;
			a.Format(_T("%c%c"),_T('A'), (_T('A') + index - 1));
			CString tmp1;
			tmp1.Format(_T("%s%d"), a, endRow);
			return tmp1;
		}
		else
		{
			TCHAR tmpchar = cellBegin[0] + 1;
			int index = (beginChar + cols - 1)% _T('Z');
			CString a;
			a.Format(_T("%c%c"), tmpchar, (_T('A') + index - 1));
			CString tmp1;
			tmp1.Format(_T("%s%d"), a, endRow);
			return tmp1;
		}
	}
	CString tmp;
	if(!hasTwoChar)
		tmp.Format(_T("%c%d"), endChar, endRow);
	else
		tmp.Format(_T("%c%c%d"), cellBegin[0], endChar, endRow);
	return tmp;
}

//��ȡһƬ��Χ��ֵ
//cellBegin �������
//rows ����
//cols ����
void ExcelProcess::getValue( vector<vector<CString>>& outData, CString cellBegin, int rows, int cols)
{
	CString cellEnd = getEndCell(cellBegin, rows, cols);
	range = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	VARIANT data = range.get_Value2();  
	if(rows == 1 && cols == 1)
	{
		vector<CString> tmp;
		tmp.push_back(data.bstrVal);
		outData.push_back(tmp);
		return;
	}
	COleSafeArray safeArr(data);
	
// 	COleSafeArray safeArr;
// 	DWORD numElements[] = {rows, cols};
// 	safeArr.Create(VT_BSTR , 2, numElements);
// 	safeArr = (COleSafeArray)data;

	long index[2] = {0, 0};
	long lFirstLBound = 0;
	long lFirstUBound = 0;
	long lSecondLBound = 0;
	long lSecondUBound = 0;
	//��ȡ�е�����
	safeArr.GetLBound(1, &lFirstLBound);
	//��ȡ�е�����
	safeArr.GetUBound(1, &lFirstUBound);
	//��ȡ�е�����
	safeArr.GetLBound(2, &lSecondLBound);
	//��ȡ�е�����
	safeArr.GetUBound(2, &lSecondUBound);
	VARIANT value;
	//����
	for(long i = lFirstLBound; i <= lFirstUBound; i++)
	{
		index[0] = i;
		vector<CString> tmp;
		for(long j = lSecondLBound; j <= lSecondUBound; j++)
		{
			index[1] = j;
			safeArr.GetElement(index, &value);
			CString valueStr;
			switch (value.vt)
			{
			case VT_BOOL:
				{
					BOOL t = value.boolVal;
					valueStr = t ? _T("TRUE") : _T("FALSE");
					break;
				}
			case VT_BSTR:
				{
					valueStr = value.bstrVal;
					break;
				}
			case VT_I4:
				{
					int t = value.intVal;
					valueStr.Format(_T("%d"), t);
					break;
				}
			case VT_R8:
				{
					double t = value.dblVal;
					valueStr.Format(_T("%lf"), t);
					break;
				}
			default:
				break;
			}

			tmp.push_back(valueStr);
		}
		outData.push_back(tmp);
		tmp.clear();
	}
}

/************************************************************************/
/* ����                                                                 */
/************************************************************************/
void ExcelProcess::saveExcel()
{
	ExcelApp.put_DisplayAlerts(FALSE);
	//book.Close(vtMissing, vtMissing, vtMissing);
	book.Save();
}

//�ر�
void ExcelProcess::closeExcel()
{
	//ExcelApp.get_ThisWorkbook();
	COleVariant SaveChanges((short)FALSE), RouteWorkbook((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	book.Close(SaveChanges, vtMissing, RouteWorkbook);
}

/************************************************************************/
/* ���Ϊ                                                               */
/************************************************************************/
void ExcelProcess::saveExcelAs( CString savePath )
{
	savePath.Trim();
	book.SaveAs(_variant_t(savePath), 
		vtMissing, vtMissing, vtMissing ,vtMissing,vtMissing, 0,
		vtMissing,vtMissing,vtMissing,vtMissing, vtMissing);
}

/************************************************************************/
/* ���Ϊxml                                                            */
/************************************************************************/
void ExcelProcess::savaExcelToXml( CString savePath )
{
	savePath.Trim();
	CString str = savePath.Right(3);
	str.MakeLower();
	if(str != _T("xml"))
	{
		//AfxMessageBox(_T("�����ļ�������"));
		return;
	}
	book.SaveAs(_variant_t(savePath), _variant_t(XlFileFormat::xlXMLSpreadsheet), 
		vtMissing, vtMissing, _variant_t(FALSE),_variant_t(FALSE), 0,
		vtMissing, vtMissing, vtMissing, vtMissing, vtMissing);
}

//�ֽ�comBox
//comBoxΪstruct.xml�е�opt
BOOL ExcelProcess::splitComBox( CString comBox, std::vector<CString>& strList )
{
	int indexFirst;
	int indexSecond;
	while(1)
	{
		indexFirst = comBox.Find(_T(':'));
		indexSecond = comBox.Find(_T(";"));
		CString tmp = comBox.Mid(0, indexSecond);
		CString key = tmp.Left(indexFirst);
		CString value = tmp.Right(tmp.GetLength() - 1 - indexFirst);
		strList.push_back(value);
		comBox = comBox.Right(comBox.GetLength() - 1 - indexSecond);
		if(comBox.GetLength() == 0)
			break;
	}
	return TRUE;
}
//�����ｫcombox��߿���key,�ұ߿���value��key��value��һ��һ�ģ����ݴ����key��ȡvalue
BOOL ExcelProcess::getComBoxValue( CString comBox, const CString& key, CString& value )
{
	int indexFirst;
	int indexSecond;
	while(1)
	{
		indexFirst = comBox.Find(_T(':'));
		indexSecond = comBox.Find(_T(";"));
		CString tmp = comBox.Mid(0, indexSecond);
		CString keyStr = tmp.Left(indexFirst);
		CString valueStr = tmp.Right(tmp.GetLength() - 1 - indexFirst);
		if(keyStr == key)
		{
			value = valueStr;
			return TRUE;
		}
		comBox = comBox.Right(comBox.GetLength() - 1 - indexSecond);
		if(comBox.GetLength() == 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

//�����ｫcombox��߿���key,�ұ߿���value��key��value��һ��һ��,ͨ��value����key��ʹ���ڵ��빦��
BOOL ExcelProcess::getComBoxKey( CString comBox, const CString& value, CString& key  )
{
	int indexFirst;
	int indexSecond;
	while(1)
	{
		indexFirst = comBox.Find(_T(':'));
		indexSecond = comBox.Find(_T(';'));
		CString tmp = comBox.Mid(0, indexSecond);
		CString keyStr = tmp.Left(indexFirst);         
		CString valueStr = tmp.Right(tmp.GetLength() - 1 - indexFirst);
		if(valueStr == value)
		{
			key = keyStr;
			return TRUE;
		}
		int index = valueStr.Find(value);
		if(index != -1 && index >= 2)
		{
			if(isNumber(valueStr.Left(index - 1)))
			{
				key = keyStr;
				return TRUE;
			}
		}
		comBox = comBox.Right(comBox.GetLength() - 1 - indexSecond);
		if(comBox.GetLength() == 0)
		{
			return FALSE;
		}
	}
	return FALSE;
}

//�Ӵ�ָ����Χ�ڵ�Ԫ�������
BOOL ExcelProcess::setCellsBold( CString cellBegin, CString cellEnd, BOOL bold )
{
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	LPDISPATCH result = range.get_Font();
	font.AttachDispatch(result);
	font.put_Bold(_variant_t(bold));
	return TRUE;
}

//����ָ����Χ�ڵ�Ԫ�������
BOOL ExcelProcess::setCellsFont( CString cellBegin, CString cellEnd, CString fontName, int fontSize, BOOL bold /*= FALSE*/ )
{
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	LPDISPATCH result = range.get_Font();
	font.AttachDispatch(result);
	font.put_Bold(_variant_t(bold));
	font.put_Name(_variant_t(fontName));
	font.put_Size(_variant_t(fontSize));

	//office 2003��֧�֣�����ע�͵� ��Ӱ����
	/*font.put_Strikethrough(_variant_t(FALSE));
	font.put_Superscript(_variant_t(FALSE));
	font.put_Subscript(_variant_t(FALSE));
	font.put_OutlineFont(_variant_t(FALSE));
	font.put_Shadow(_variant_t(FALSE));
	font.put_Underline(_variant_t(Excel::XlUnderlineStyle::xlUnderlineStyleNone));
	font.put_ThemeColor(_variant_t(XlThemeColor::xlThemeColorLight1));
	font.put_TintAndShade(_variant_t(0));
	font.put_ThemeFont(_variant_t(XlThemeFont::xlThemeFontNone));*/
	return TRUE;
}


//�����к�����������ɫ
BOOL ExcelProcess::setRowColor( UINT rowIndex, int colorIndex )
{
	CString strRowIndex;
	strRowIndex.Format(_T("%d:%d"), rowIndex, rowIndex);
	lpDisp = sheet.get_Range(_variant_t(strRowIndex), vtMissing);
	range.AttachDispatch(lpDisp);
	LPDISPATCH tmp = range.get_Interior();
	interior.AttachDispatch(tmp);
	interior.put_ColorIndex(_variant_t(colorIndex));
	return TRUE;
}

//���ݵ�Ԫ���ȡ���к�
long ExcelProcess::getCellRowIndex( CString cellIndex )
{
	int i = 0;
	for(i = 0; i < cellIndex.GetLength(); i++)
	{
		if(cellIndex.GetAt(i) >= '0' && cellIndex.GetAt(i) <= '9')
			break;
	}
	return _ttoi(cellIndex.Mid(i, cellIndex.GetLength() - i));
}

//���õ�����Ԫ���ֵ
void ExcelProcess::setCellValue( CString cellIndex, CString value )
{
	range = sheet.get_Range(_variant_t(cellIndex), _variant_t(cellIndex));
	range.put_Value2(_variant_t(value));
	setCellsAlignLeft(range);
}

//��ȡ������Ԫ���ֵ
CString ExcelProcess::getCellValue( CString cellIndex )
{
	range = sheet.get_Range(_variant_t(cellIndex), _variant_t(cellIndex));
	VARIANT value = range.get_Value2();
	if(value.vt == VT_R8)
	{
		int num = (int)value.dblVal;
		CString str;
		str.Format(_T("%d"), num);
		return str;
	}
	return value.bstrVal;
}

//���õ�Ԫ���ʽ
void ExcelProcess::setCellsFormat( CString cellBegin, CString cellEnd, CString format )
{
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	range.put_NumberFormatLocal(_variant_t(format));
}

//���õ�Ԫ�������
void ExcelProcess::setCellsAlignLeft(CRange range )
{
	range.put_HorizontalAlignment(_variant_t(Constants::xlLeft));
	range.put_VerticalAlignment(_variant_t(Constants::xlBottom));
	range.put_WrapText(_variant_t(FALSE));
	range.put_Orientation(_variant_t(0));
	range.put_AddIndent(_variant_t(FALSE));
	range.put_IndentLevel(_variant_t(0));
	range.put_ShrinkToFit(_variant_t(FALSE));
	range.put_ReadingOrder(_variant_t(Constants::xlContext));
	range.put_MergeCells(_variant_t(FALSE));
}

//���õ�Ԫ���Ҷ���
void ExcelProcess::setCellsAlignLeft( CString cellBegin, CString cellEnd )
{
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	range.put_HorizontalAlignment(_variant_t(Constants::xlLeft));
	range.put_VerticalAlignment(_variant_t(Constants::xlBottom));
	range.put_WrapText(_variant_t(FALSE));
	range.put_Orientation(_variant_t(0));
	range.put_AddIndent(_variant_t(FALSE));
	range.put_IndentLevel(_variant_t(0));
	range.put_ShrinkToFit(_variant_t(FALSE));
	range.put_ReadingOrder(_variant_t(Constants::xlContext));
	range.put_MergeCells(_variant_t(FALSE));
}

//���õ�Ԫ���ַ�����
void ExcelProcess::setCellsLength( CString cellBegin, CString cellEnd, UINT length )
{
	CString strLength;
	strLength.Format(_T("%d"), length);
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);
	LPDISPATCH result = range.get_Validation();
	validation.AttachDispatch(result);
	validation.Delete();
	validation.Add(XlDVType::xlValidateTextLength, _variant_t(XlDVAlertStyle::xlValidAlertStop), 
		_variant_t(XlFormatConditionOperator::xlEqual),_variant_t(strLength), _variant_t(NULL));

	validation.put_IgnoreBlank(TRUE);
	validation.put_InCellDropdown(TRUE);
	validation.put_IMEMode(XlIMEMode::xlIMEModeNoControl);
	validation.put_ShowInput(TRUE);
	validation.put_ShowError(TRUE);
}

//���õ�Ԫ��������ı�����
void ExcelProcess::setCellsLength( CString cellBegin, CString cellEnd, UINT lengthMin, UINT lengthMax )
{
	CString strLengthMin;
	CString strLengthMax;
	strLengthMin.Format(_T("%d"), lengthMin);
	strLengthMax.Format(_T("%d"), lengthMax);
	lpDisp = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.AttachDispatch(lpDisp);

	LPDISPATCH result = range.get_Validation();
	validation.AttachDispatch(result);
	validation.Delete();
	validation.Add(XlDVType::xlValidateTextLength, _variant_t(XlDVAlertStyle::xlValidAlertStop), 
		_variant_t(XlFormatConditionOperator::xlBetween),_variant_t(strLengthMin), _variant_t(strLengthMax));

	validation.put_IgnoreBlank(TRUE);
	validation.put_InCellDropdown(TRUE);
	validation.put_IMEMode(XlIMEMode::xlIMEModeNoControl);
	validation.put_ShowInput(TRUE);
	validation.put_ShowError(TRUE);
	setCellsAlignLeft(range);
}

//���õ�Ԫ���ʽΪ�ı�
void ExcelProcess::setCellsToText( CString cellBegin, CString cellEnd )
{

	COleSafeArray saRet;  
	DWORD numElements = {2};//��������2��Ԫ��  
	saRet.Create(VT_I4, 1, &numElements);//��һ��������ʾ����int���ڶ���������ʾ��һά���飬������������ʾ��������2��Ԫ��  
	long index = 0;//�����±�  
	int val = 1;//ֵ  
	saRet.PutElement(&index, &val);//��0�±��ֵ����Ϊ1  
	index++;  
	val = 2;  
	saRet.PutElement(&index, &val);
	CRange tmp = sheet.get_Range(_variant_t(cellBegin), vtMissing);
	range = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.TextToColumns(_variant_t(tmp), XlTextParsingType::xlDelimited, Constants::xlDoubleQuote, 
		_variant_t(FALSE), _variant_t(TRUE), _variant_t(FALSE), _variant_t(FALSE), _variant_t(FALSE), vtMissing,
		vtMissing, _variant_t(saRet), vtMissing, vtMissing, _variant_t(TRUE));
}

//�����п�
void ExcelProcess::setColumnWidth( CString cellIndex, int width )
{
	range = sheet.get_Range(_variant_t(cellIndex), _variant_t(cellIndex));
	range.put_ColumnWidth(_variant_t(width));
}

//������Ԫ��
void ExcelProcess::unlockALL()
{
	range = sheet.get_Cells();
	//range.Activate();
	range.put_Locked(_variant_t(FALSE));
	range.put_FormulaHidden(_variant_t(FALSE));
}

//������Ԫ��
void ExcelProcess::lockCells( CString cellBegin, CString cellEnd )
{
	range = sheet.get_Range(_variant_t(cellBegin), _variant_t(cellEnd));
	range.put_Locked(_variant_t(TRUE));
	range.put_FormulaHidden(_variant_t(FALSE));
}

//����cellIndex������
void ExcelProcess::lockRow(CString cellIndex)
{
	int row = getCellRowIndex(cellIndex);
	CString format;
	format.Format(_T("%d:%d"), row, row);
	range = sheet.get_Range(_variant_t(format), vtMissing);
	range.put_Locked(_variant_t(TRUE));
	range.put_FormulaHidden(_variant_t(FALSE));
}
//����cellBegin��cellEnd������
void ExcelProcess::lockRows(CString cellBegin, CString cellEnd)
{
	int row1 = getCellRowIndex(cellBegin);
	int row2 = getCellRowIndex(cellEnd);
	CString format;
	format.Format(_T("%d:%d"), row1, row2);
	range = sheet.get_Range(_variant_t(format), vtMissing);
	range.put_Locked(_variant_t(TRUE));
	range.put_FormulaHidden(_variant_t(FALSE));
}

//����sheet��sheet�б������ĵ�Ԫ���û����޷�ѡ��
void ExcelProcess::setSheetProtect(CString sheetName)
{
	if(getSheet(sheetName))
	{
		sheet.Protect(vtMissing, _variant_t(TRUE), _variant_t(TRUE), _variant_t(TRUE), vtMissing, vtMissing, _variant_t(TRUE), vtMissing, vtMissing, 
		vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing);
		sheet.put_EnableSelection(XlEnableSelection::xlUnlockedCells);
	}
}

//�������sheet
void ExcelProcess::setSheetUnprotect()
{
	sheet.Unprotect(vtMissing);
}

//��ӹ�ʽ
void ExcelProcess::addFormula( CString name, CString formula )
{
	names.AttachDispatch(book.get_Names());
	names.Add(_variant_t(name),_variant_t(formula), vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, 
		vtMissing, vtMissing, vtMissing, vtMissing);
}

//��ȡ��ʽ
CString ExcelProcess::getFormula(CString sheetName, CString cellBegin, CString cellEnd, FormulaType type)
{
	cellBegin.Insert(0, _T("$"));
	cellEnd.Insert(0, _T("$"));
	int index = 0;
	for(index = 0; index < cellBegin.GetLength(); index++)
	{
		if(cellBegin.GetAt(index) >= _T('0') && cellBegin.GetAt(index) <= _T('9'))
			break;
	}
	cellBegin.Insert(index, _T("$"));
	index = 0;
	for(index = 0; index < cellEnd.GetLength(); index++)
	{
		if(cellEnd.GetAt(index) >= _T('0') && cellEnd.GetAt(index) <= _T('9'))
			break;
	}
	cellEnd.Insert(index, _T("$"));

	CString formula;
	if(type == FormulaType::cellFromRange)
	{
		CString count;
		count.Format(_T("SUMPRODUCT(N(LEN(%s!%s:%s)>0))"), sheetName, cellBegin, cellEnd);
		//��ʽ�����õ�Ԫ���ȡֵ��һ����Χ��������ֵ����ȥ��ֵ
		formula.Format(_T("=OFFSET(%s!%s,,,SUMPRODUCT(N(LEN(%s!%s:%s)>0)),)"), sheetName, cellBegin, sheetName, cellBegin, cellEnd);
		formula.Format(_T("=OFFSET(%s!%s,,,IF(%s > 0, %s, 1),)"), 
			sheetName, cellBegin, count, count);
	}
	return formula;
}

//��A1��Ԫ��ʼ�����ķ�Χ
//row��column����һ��Ԥ��ֵ
void ExcelProcess::getMaxRange(UINT& row, UINT& column)
{
	CString beginCellIndex = _T("A1");
// 	if(row >= 2 && column >= 1)
// 		beginCellIndex = getEndCell(beginCellIndex, row + 1, column);
// 	CString beginCellIndexTmp = beginCellIndex;
	//�����
	//TODO ���������м�� �������϶��ʱ��Ч�ʽϵͣ����������Ż�
	while(1)
	{
		if(!getCellValue(beginCellIndex).IsEmpty())
		{
			row++;
			beginCellIndex = getEndCell(beginCellIndex, 2, 1);
			
		}
		else
		{
			break;
		}
	}
	//�����
	//TODO ���������м�� �������϶��ʱ��Ч�ʽϵͣ����������Ż�
	//beginCellIndex = beginCellIndexTmp;
	beginCellIndex = _T("A1");
	while(1)
	{
		if(!getCellValue(beginCellIndex).IsEmpty())
		{
			column++;
			beginCellIndex = getEndCell(beginCellIndex, 1, 2);

		}
		else
		{
			break;
		}
	}
}