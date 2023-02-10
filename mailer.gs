function findEmail(current_row){//last entered row number is passed
  //get 2 sheets containg live data and lookup data containing mailing list
  const sheet_id = '1Zlce6x71hMGR_s6yEej6jAm2shXdK09ecI-1eBI7hcg'; // Spreadsheet ID
  const sheetName="Lookup"
  const sheet_1=SpreadsheetApp.openById(sheet_id).getSheetByName("main");
  const sheet_2 = SpreadsheetApp.openById(sheet_id).getSheetByName(sheetName);
  var user={};
  var datetime="";
  //get the value of student id in 3rd column using row number that is passed
  const clientName = sheet_1.getRange("C"+current_row).getDisplayValue();
  //get the range of cells of ID to compare the student id in the mailing list
  //start in row 2, column 1 and get all rows expect the first (=> last - 1) and get 2 columns
  const range = sheet_2.getRange(2, 1, sheet_2.getLastRow() - 1, 1);
  // use forEach() and array destructuring 
  range.getDisplayValues().forEach(([name,email_id], idx) => {
    //if student id matches with list of ids in mailing list
    if(name === clientName){
      //set status in sheet 1 to sending
      sheet_1.getRange(current_row,5).setValue("Sending..");
      //get the display value of date
      datetime=sheet_1.getRange("A"+current_row).getDisplayValue();
      //get the display value of time in sheet and combine with date as string
      datetime=datetime+" "+sheet_1.getRange("B"+current_row).getDisplayValue();
      user.datetime=datetime;
      //store the name, email_id and mess_id of the stundent ID that matched in the structure
      user.roll_id=name;
      user.mail_id=email_id;
      user.mess_id=sheet_1.getRange("D"+current_row).getDisplayValue();
      //call the function sendemail by passing the structure "user"
      sendEmail(user);
      //set status in sheet 1 to sent
      sheet_1.getRange(current_row,5).setValue("Sent");
    }
  })
  
}

function sendEmail(user){
  const body = HtmlService.createTemplateFromFile("email");
  body.user=user
  MailApp.sendEmail({
  to: user.mail_id,
  subject: 'Dine-ins',
  htmlBody: body.evaluate().getContent(),
      });
}
