function doGet(e) { 
  Logger.log( JSON.stringify(e) );
  var result = 'Ok';
  //if any parameters are not received then do nothing
if (e.parameter == 'undefined') {
  result = 'No Parameters';
}
//when parameters are recieved
else {
  var sheet_id = '1Zlce6x71hMGR_s6yEej6jAm2shXdK09ecI-1eBI7hcg'; // Spreadsheet ID
  //Open main spreadsheet where data is to be logged
  var sheetName="main"
  var sheet = SpreadsheetApp.openById(sheet_id).getSheetByName(sheetName);
  //get the last valid row
  var newRow = sheet.getLastRow() + 1; 
  var rowData = [];
  //get the current date and time from built-in functions
  var Curr_Date = new Date();
  rowData[0] = Curr_Date; // Date in column A
  var Curr_Time = Utilities.formatDate(Curr_Date, "Asia/Kolkata", 'HH:mm:ss');
  rowData[1] = Curr_Time; // Time in column B
  //In the parameteres recieved, loop through them
  for (var param in e.parameter) {
    var value = stripQuotes(e.parameter[param]);
    //parameters are in the form id="xxxxxx" and mess="x", so use switch case while looping through them
    switch (param) {
      case 'id':
      //Enter Roll no list of rowData
        rowData[2] = value; // ID in column C
        result = 'ID Written on column C'; 
        break;
      case 'mess':
      //Enter Mess into list of rowData
        rowData[3] = value; // Mess in column D
        result += ' , Mess Written on column D'; 
        break; 
      default:
      //if any other parameter is encountered don't enter and change result
        result = "unsupported parameter";
    }
  }
  // get cells from the current working row, column 1, get only 1 row, and get 4 columns as rowdata list size is 4 
  var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
  //set the elements in the list in each column of the newrow i.e 4 columns with 4 elements
  newRange.setValues([rowData]);
  //After entering the data, find the corresponding mail_id and relevant data of the current row
  findEmail(sheet.getLastRow());
}
return ContentService.createTextOutput(result);
}
function stripQuotes( value ) {
return value.replace(/^["']|['"]$/g, "");
}
