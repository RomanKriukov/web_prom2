<SCRIPT LANGUAGE=JScript RUNAT=Server>
function ConStr() { 
	if ((Session.Contents('UserId')=='')||(Session.Contents('Password')=='')) {
		if (''+Request.Form("UID")!='undefined') {
			var s=''+Request.Form("UID");
			Session.Contents("UserId")=s;

			if (''+Request.Form("PWD")!='undefined') {
				var s=''+Request.Form("PWD");
				if (s!='') 
					Session.Contents("Password")=s;
			}
			if (''+Request.Form("ECS")!='undefined') {
				s=''+Request.Form("ECS");
				if (s!='') {
					s=s.substr(22);
					s=s.substring(0, s.length-9);
					var enc='';
					for (i=0;i<s.length;i++) {
						var d=s.charCodeAt(i);
						d=d ^ 123;
						enc+=String.fromCharCode(d);
					}
					Session.Contents("Password")=(enc);
				}
			}
		}
	}
	if ((Session.Contents("UserId")=="")||(Session.Contents("Password")==""))
		return ''
	return Session.Contents("ConStr")+Session.Contents("Database")+
		";Password="+ Session.Contents("Password") + ";User ID=" + Session.Contents("UserId")
}

function getConnection(newconnection) 
{ 
	oConn=Session.Contents("Connection");
	if ((newconnection)&(oConn.State>0))
		oConn.Close();
	if (oConn.State==0)
	{
		var s=ConStr();
		if (s!='')
		{
			try {
				oConn.Open(s);
			} 
			catch (e) {
			sayerror('',e)
			}
		}
	}
	return oConn;
}

function sayerror(msg, e)
{
Response.Write('<div class=error>'+msg+'</div>')
if (e!=null)
  Response.Write('<div class=error>'+e.message+'</div>')
}
function zamena(stf,old,rplc) {
var New='';
while (stf.indexOf(old)!=-1) {
var strt=stf.indexOf(old);
New+=stf.substring(0,strt);
New+=rplc;
stf=stf.substring(strt+old.length,stf.length);
} return New+stf;
}
function dateFormat(ds,type) {
 if (ds==null) {return ds}
 else {
 dt=new Date(Date.parse(ds));
 m=dt.getMonth()+1;
 if (m<10) {m='0'+m} else {m=''+m};
 d=dt.getDate();
 if (d<10) {d='0'+d} else {d=''+d};
 if (type==999135)
   return dt.getFullYear()+'.'+m+'.'+d+' '+dt.getHours()+':'+dt.getMinutes()+':'+dt.getSeconds()//+'.'+dt.getMilliseconds()
 else
   return d+'.'+m+'.'+dt.getFullYear()
 }
}
function dateToANSI(dt) {
// dd.mm.yyyy -> yyyymmdd
  return dt.substr(6,4)+dt.substr(3,2)+dt.substr(0,2)
}
function fieldStr(fld) {
var t=fld.Type;
return ((((t==7)||(t==135)||(t==133))?dateFormat(fld.Value,t):((fld.Value==null?'-':(zamena(''+fld.Value,"'",""))))))
}
function translate(lex){
  switch (lex) {
   case 'comment': return 'комментарий';
   case 'contragent': return 'контрагент';
   case 'credit': return 'кредит';
   case 'currencyname': return 'валюта';
   case 'currentprice': return 'учетная цена';
   case 'debet': return 'дебет';
   case 'divisname': return 'подразделение';
   case 'docdate': return 'дата';
   case 'dt': return 'дата';
   case 'id': return 'код';
   case 'kind': return 'тип';
   case 'name': return 'название';
   case 'oper': return 'операция';
   case 'opername': return 'операция';
   case 'originalnum': return 'номер';
   case 'price': return 'цена';
   case 'qsaldo': return 'остаток,количество';
   case 'quantity': return 'количество';
   case 'sk': return 'остаток,сумма';
   case 'sn': return 'остаток,сумма';
   case 'ssaldo': return 'остаток,сумма';
   case 'summa': return 'сумма';
   case 'typename': return 'тип документа';
  }
  return lex;
}

function printTable(oRs,link,cmd,list) {
if (cmd==null) cmd='exec';
if (oRs.State!=0) {
Response.write('<table class="sort-table" cellspacing="1" cellpadding="1">')
var wrap=(cmd=='sel')?' nowrap':'';
Response.write('<thead>')
var rec=0;
if ((list==null)||(list=='')) {
  list = [];
  for(i=0;i<oRs.fields.count;i++) {
    var f = oRs(i);
    if (f.Type!=72)
      list[list.length]=f.Name;
  }
}
if ((link!=null)&&(oRs(link)==null)) link=null;
for(i=0;i<list.length;i++) {
    var f = oRs(list[i]);
    if (f!=null) {
//      if (f.Name==link) rec=i;
      Response.write('<td><span id="t_info_'+i+'">'+translate(f.Name)+'</span></td>');
    } //else Response.write('<td>'+translate(list[i])+'</td>');
}

Response.write('</thead><tbody>')
while (!oRs.eof) 
{
   rec++;
   Response.write('<tr valign="top" '+(rec % 2 ==0?'class="odd"':'')+'>');
   if (link!=null) Response.Write('<span id="r'+oRs(link)+'">');
   for(j=0;j<list.length; j++) {
     var f = oRs(list[j]);
     if (f!=null)
     {
     t=f.Type;
     Response.write('<td')
     if ((t==20)||(t==6)||(t==14)||(t==5)||(t==3)||(t==131)||(t==4)||(t==2)||(t==16))
       Response.write(' align=right nowrap')
     else
       Response.write(wrap);
     Response.write('>')
     if ((link!=null)&&(f.Name==link))
       Response.write('<a href=javascript:'+cmd+'("'+f.value+'")>');
     Response.write(fieldStr(f));
     if ((link!=null)&&(f.Name==link))
       Response.write('</a>')
     Response.write('</td>')
     }
   }
   if (link!='') Response.write('</span>')
   oRs.MoveNext();
   i++;
} 
Response.write('</table>')
}
}

function BuildMenu(url) {
var result='';
if (url!='default') {result+='<li><a href="default.asp">Главная</a>'};
//if (Session.Contents("Isboss")==true) 
s=(''+Session.Contents("Password"));
if (s!='') { 
  result+='<li><a href="logout.asp">Выйти</a>';
  if (url=='default') {result+='<li><a href="newpwd.asp">Сменить пароль</a>';};

  if(Session.Contents("Isalienuser2")!=true){
  if (Session.Contents("Istopmanager")==true ||  Session.Contents("UserId").toLowerCase() =='gridin' ||  Session.Contents("UserId").toLowerCase() =='dudnikova' ||  Session.Contents("UserId").toLowerCase() =='bonadareadmin' || Session.Contents("UserId").toLowerCase() =='ivchenko' ||  Session.Contents("UserId").toLowerCase() =='voloshinm' ||  Session.Contents("UserId").toLowerCase() =='yashulin' || Session.Contents("UserId").toLowerCase() =='poshtar' || Session.Contents("UserId").toLowerCase() =='harchenkon' || Session.Contents("UserId").toLowerCase() == 'autobuh2' || Session.Contents("UserId").toLowerCase() == 'finuser') 
  { 
	  result+='<li><a href="delta2SZRAgro.asp" target="_blank">Не списанный НДС (ДТ, Агромаркет, СЗР, Удобрения)</a>'; 
	  result+='<li><a href="repExpNDS.asp" target="_blank">Просроченный НДС</a>';
	  result+='<li><a href="delta2.asp" target="_blank">Дельта</a>'; 
	  if(Session.Contents("Issysadm")==true || Session.Contents("UserId").toLowerCase() =='kermakov' || Session.Contents("UserId").toLowerCase() =='grv' || Session.Contents("UserId").toLowerCase() =='revizor' || Session.Contents("UserId").toLowerCase() =='ivchenko'){ 
	  	result+='<li><a href="liquidityReport.asp" target="_blank">Ликвидность</a>';  
	  }
	  if (/*Session.Contents("UserId").toLowerCase()=='grv' ||*/ Session.Contents("Issysadm")==true){
		result+='<li><a href="deltaChange.asp" target="_blank">Изменение дельты</a>';  
	  }  
	  //result+='<li><a href="delta2TestNDS.asp" target="_blank">Дельта(ТЕСТ)</a>';
  }  
  
  result+='<li><a href="getTopCommandments.asp" target="_blank">ТОП Заповедей по отделам</a>'; 
   if (Session.Contents("Issysadm")==true || Session.Contents("Istopmanager")==true || Session.Contents("UserId").toLowerCase()=='starikov' || Session.Contents("UserId").toLowerCase()=='saulina' || Session.Contents("UserId").toLowerCase()=='hrobatenko' ||  Session.Contents("UserId").toLowerCase() =='dudnikova' ||  Session.Contents("UserId").toLowerCase() =='voloshinm' ||  ((Session.Contents("Isboss")==true) && (Session.Contents("Ismanager")==true) && (Session.Contents("IsaccountantCanSee622")==null)) || Session.Contents("UserId").toLowerCase() == 'autobuh2') 
  {			
		//result+='<li><a href="costincomnewWithoutFarms.asp" target="_blank">Факт и выполнение финплана (БЕЗ ХОЗЯЙСТВ)</a>'; // Прибыль текущая Элеваторы
		result+='<li><a href="costincomnewWithoutFarms2.asp" target="_blank">KPI (БЕЗ ХОЗЯЙСТВ)</a>'; // Прибыль текущая Элеваторы				
		if (Session.Contents("UserId").toLowerCase()!='hrobatenko'){
			result+='<li><a href="costincomnewFarms.asp" target="_blank">KPI (ХОЗЯЙСТВА)</a>'; // Прибыль текущая ХОЗЯЙСТВА			
		}
		//result+='<li><a href="costincomnew.asp" target="_blank">Прибыль текущая</a>';	// Удалено 27.08.2019, вернули 04.11.2019, убрали окончательно 2020 // Таки вернул 04.02.2021   //убрал окончательно 25.01.2022
		result+='<li><a href="costincomPlan.asp" target="_blank">План Бюджет</a>'; 
		result+='<li><a href="repFinTransactions.asp" target="_blank">Финансовые операции</a>'; 		
		if (Session.Contents("UserId").toLowerCase()=='gridin' || Session.Contents("UserId").toLowerCase()=='kermakov'){
			result+='<li><a href="repPlanFact.asp" target="_blank">Текущее выполнение финплана</a>';	
			result+='<li><a href="repBreakEvenPoint.asp" target="_blank">Точка безубыточности</a>';
		}
  }
  if (Session.Contents("Issysadm")==true || Session.Contents("UserId").toLowerCase()=='grv') 
  {					
		result+='<li><a href="costincomnewGRV.asp" target="_blank">Личные расходы РВ</a>';				
  }
  if (Session.Contents("Issysadm")==true || Session.Contents("Isrevizors")==true || Session.Contents("UserId").toLowerCase()=='avdeenko') 
  {					
		result+='<li><a href="repAdminCosts.asp" target="_blank">Админ. расходы</a>';		
  }
  if (Session.Contents("Issysadm")==true || Session.Contents("UserId").toLowerCase()=='kermakov'  || Session.Contents("UserId").toLowerCase()=='revizor')
  {					
		//result+='<li><a href="repDebetorsDeltaEXP.asp" target="_blank">Просроченная дебеторская задолженность (договора, контракты)</a>';				
  }
  if (Session.Contents("Istopmanager")==true || ((Session.Contents("Isboss")==true) && (Session.Contents("Ismanager")==true) && (Session.Contents("Isaccountantcansee622")==null)) || Session.Contents("UserId").toLowerCase() == 'autobuh2')   //убрал 25.01.2022   //вернул 01.02.2022
	{	            
		result+='<li><a href="costincomprofit.asp" target="_blank">KPI (ТРЕЙДЕРА)</a>';
		//result+='<li><a href="auto3.asp"  target="_blank">Содержание авто</a>';
	}

	if (Session.Contents("Istopmanager")==true || Session.Contents("UserId").toLowerCase()=='hrobatenko' || ((Session.Contents("Isboss")==true) && (Session.Contents("Ismanager")==true)) || (Session.Contents("UserId").toLowerCase() =='fedoruk') || (Session.Contents("Istreaty")==true) || (Session.Contents("Isjurist")==true) || Session.Contents("UserId").toLowerCase() == 'autobuh2')
	{
	    result+='<li><a href="repDebetorsDelta.asp" target="_blank">Дебиторы (просроченные и текущие)</a>';
		result+='<li><a href="repCreditDebtElev.asp" target="_blank">Долги за товар по элеваторам Ф1</a>';
		result+='<li><a href="repNaklToPay.asp" target="_blank">Долги за товар по элеваторам Ф2</a>';

		/*if(Session.Contents("Issysadm") == true){
			result+='<li><a href="repCreditorsWeb.asp" target="_blank">Долги за товар Ф1</a>';
			
			result+='<li><a href="repCreditorsPort61.asp" target="_blank">Долги по Ф2 на порты</a>';
		}*/
	}
	//if (Session.Contents("Istreaty")==true) || (Session.Contents("Isjurist")==true) {
	//	result+='<li><a href="repDebetorsDelta.asp" target="_blank">Просроченные дебеторы</a>';
	//}
	if(Session.Contents("Issysadm")==true  || Session.Contents("Iseconomist")==true || Session.Contents("Islogist")==true ||  Session.Contents("UserId").toLowerCase() == 'revizor'||  Session.Contents("UserId").toLowerCase() == 'revizor2' || Session.Contents("UserId").toLowerCase() == 'kermakov' || Session.Contents("UserId").toLowerCase() == 'autobuh2'){
		result+='<li><a href="debtorsForRevizors.asp" target="_blank">Просроченные дебеторы для ревизоров</a>'; // добавлено 13.12.2021
	}
	if (Session.Contents("Istopmanager")==true || Session.Contents("Ismanager")==true || Session.Contents("Ispricer") || Session.Contents("UserId").toLowerCase() == 'autobuh2') 
	{
		result+='<li><a href="perevalka.asp" target="_blank">Заход на элеваторы (и переоформления)</a>';  
	}
  /*if (Session.Contents("Isbank")==true) {       //убрал 25.01.2022
	  result+='<li><a href="cf.asp" target="_blank">Кэш флоу (TEST)</a>';
    //result+='<li><a href="sbd.asp" target="_blank">Товарные остатки</a>';
  }*/
  if (Session.Contents("Ismanager")==true) 
  { 
	  //    result+='<li><a href="repContractQuota.asp" target="_blank">Квоты</a>';
	  //    result+='<li><a href="naklinget.asp" target="_blank">Приходные накладные</a>';
	  //    result+='<li><a href="bankinget.asp" target="_blank">Поступление на счет</a>';
	  //    result+='<li><a href="bankoutget.asp" target="_blank">Оплата со счета</a>';
    result+='<li><a href="acc71get.asp" target="_blank">Подотчетники</a>';
	  //    result+='<li><a href="elevatorsDebt.asp" target="_blank">Задолженность по Ф2</a>';
  };  	    
	if (Session.Contents("UserId").toLowerCase()=='gudyma' || Session.Contents("UserId").toLowerCase()=='grv' || Session.Contents("UserId").toLowerCase()=='bondaruk' || Session.Contents("UserId").toLowerCase()=='dalakian' || Session.Contents("UserId").toLowerCase()=='yashulin' || Session.Contents("UserId").toLowerCase() == 'autobuh2'){
		result+='<li><a href="elevSF.asp" target="_blank">Остатки на элеваторе (подсолнечник - все виды)</a>'; 
	}   
	if (Session.Contents("UserId").toLowerCase()=='gudyma' || Session.Contents("UserId").toLowerCase()=='akopyan' || Session.Contents("UserId").toLowerCase()=='grv' ||(Session.Contents("Issysadm")==true) || Session.Contents("UserId").toLowerCase() == 'autobuh2'){
		result+='<li><a href="elevAgroTrade.asp" target="_blank">Остатки по группам культур (Агро/Трейдера)</a>'; // добавлено 05.07.2021
	}
	if (Session.Contents("UserId").toLowerCase()!='hrobatenko' || Session.Contents("UserId").toLowerCase() == 'autobuh2'){
		result+='<li><a href="elevSalt.asp" target="_blank">Остатки СОЛЬ</a>';
		result+='<li><a href="repPortRests.asp" target="_blank">Остатки товара в портах (таблица)</a>';
		result+='<li><a href="repPortRestsGraph.asp" target="_blank">Остатки товара в портах (график)</a>';
		result+='<li><a href="elev.asp" target="_blank">Остатки на элеваторах (товар Прометея)</a>';
		result+='<li><a href="elev.asp?type=1" target="_blank">Остатки на элеваторах регионы</a>';		
		result+='<li><a href="elevAnother.asp" target="_blank">Остатки на элеваторах по чужим хранителям</a>';	// добавлено 27.09.2019
		result+='<li><a href="repAmountGoodsFarms2.asp" target="_blank">Остатки на элеваторах (товар наших хозяйств)</a>';		// добавлено 27.01.2023
	}

	if (Session.Contents("UserId").toLowerCase()=='gudyma' || Session.Contents("Issysadm")==true || Session.Contents("UserId").toLowerCase()=='grv' || Session.Contents("UserId").toLowerCase()=='revizor' ||  Session.Contents("UserId").toLowerCase() == 'revizor2' || Session.Contents("UserId").toLowerCase() == 'kermakov' || Session.Contents("UserId").toLowerCase()=='naira' || (Session.Contents("Isbank")==true)  || Session.Contents("UserId").toLowerCase() == 'autobuh2'){
		result+='<li><a href="elevGG.asp" target="_blank">Остатки на элеваторах своего товара (по группам культур)</a>'; 
	}
	if (Session.Contents("UserId").toLowerCase()!='hrobatenko' || Session.Contents("UserId").toLowerCase() == 'autobuh2'){
		result+='<li><a href="elevFerRests.asp" target="_blank">Остатки нашего товара на элеваторах (с/х, удобрения, ДТ, СЗР)</a>';	// добавлено 10.10.2019
	}
	//result+='<li><a href="elevRestsUnits.asp" target="_blank">Остатки на элеваторах с разделением по таре</a>'; // добавлено 080819 и удалено 27.08.2019
	//result+='<li><a href="elevFilt.asp" target="_blank">Остатки удобрений</a>';
	if ((Session.Contents("Isboss")==true)||(Session.Contents("Islogist")==true)||(Session.Contents("Ismanager")==true) || Session.Contents("UserId").toLowerCase() == 'autobuh2') {			  
		result+='<li><a href="elevFerElevInAll.asp" target="_blank">Поступления на элеваторы (все культуры)</a>'; // добавлено 08.09.2020
		result+='<li><a href="elevin.asp" target="_blank">Поступления на элеваторы (нал/безнал)</a>';	  
		//result+='<li><a href="elev41.asp" target="_blank">Остатки чужого товара на элеваторах</a>';	// убрано 01.10.2019
		if (Session.Contents("UserId").toLowerCase() !='grv') {
			result+='<li><a href="repContractQuota.asp" target="_blank">Квоты</a>';
		}
	}  
	if (Session.Contents("Issysadm")==true  ||  Session.Contents("Isboss")==true || Session.Contents("Isdispatcher")==true || Session.Contents("UserId").toLowerCase() == 'autobuh2') {
		if (Session.Contents("UserId").toLowerCase()!='hrobatenko'){
			result+='<li><a href="repProfitAuto.asp" target="_blank">Прибыль по автомобилям</a>'; 
		}
  		result+='<li><a href="repDeliveryProfit.asp" target="_blank">Прибыль при продаже авто</a>';	// добавлено 08.09.2020
		if (Session.Contents("Issysadm")==true){
			result+='<li><a href="repProfitWagon.asp" target="_blank">Прибыль по вагонам</a>'; 
		}
	}	
	if ((Session.Contents("Isboss")==true)||(Session.Contents("Ismanager")==true)||(Session.Contents("Isbuch")==true)||(Session.Contents("Isbank")==true)||(Session.Contents("Islogist")==true)||(Session.Contents("Iselevators")==true) || Session.Contents("UserId").toLowerCase() == 'autobuh2') 
	{
		result+='<li><a href="prices.asp" target="_blank">Закупочные цены</a>';
		result+='<li><a href="pricesarc.asp" target="_blank">Архив закупочных цен</a>';
	};
	//if (Session.Contents("Iselevators")==true) 
	//{
	//    result+='<li><a href="prices.asp" target="_blank">Закупочные цены</a>';
	//    result+='<li><a href="pricesarc.asp" target="_blank">Архив закупочных цен</a>';
	//}	
	if ((Session.Contents("Isboss")==true)||(Session.Contents("Islogist")==true)||(Session.Contents("Ismanager")==true) || Session.Contents("UserId").toLowerCase() == 'autobuh2') {
		result+='<li><a href="conman.asp" target="_blank">Поставки на контракты (торговцы и элеваторы)</a>';	
	}  
	if (Session.Contents("Issysadm")==true  ||  Session.Contents("UserId").toLowerCase()=='grv' ||  Session.Contents("UserId").toLowerCase()=='revizor' ||  Session.Contents("UserId").toLowerCase() == 'revizor2' || Session.Contents("UserId").toLowerCase() == 'kermakov' ||  Session.Contents("UserId").toLowerCase()=='goroyand' || Session.Contents("UserId").toLowerCase() == 'autobuh2'){	
		result+='<li><a href="costsAll.asp" target="_blank">Затраты</a>'; // добавлено 07.12.2021
	}
	if (Session.Contents("Issysadm")==true  ||  Session.Contents("UserId").toLowerCase()=='grv' ||  Session.Contents("UserId").toLowerCase()=='revizor' ||  Session.Contents("UserId").toLowerCase() == 'revizor2' || Session.Contents("UserId").toLowerCase() == 'kermakov' || Session.Contents("UserId").toLowerCase() == 'autobuh2'){	
		result+='<li><a href="repConstructions.asp" target="_blank">Затраты на стройки</a>'; // добавлено 08.09.2020
	}	
	if ((Session.Contents("Isboss")==true)||(Session.Contents("Islogist")==true)||(Session.Contents("Ismanager")==true)) {
		result+='<li><a href="repRating.asp" target="_blank">Рейтинг торговцев по закупке</a>'; // добавлено 02.10.2020
		result+='<li><a href="repRatingFer.asp" target="_blank">Рейтинг торговцев по продаже удобрений</a>';
		result+='<li><a href="repRatingSalt.asp" target="_blank">Рейтинг торговцев по продаже соли</a>';
		result+='<li><a href="elevoutNalBeznalAndBarter.asp" target="_blank">Реализация удобрений и дизтоплива<br>(нал/безнал, бартер/не бартер)</a>';	// добавлено 29.07.2019, изменено 23.10.2019
	}
	if ((Session.Contents("Isboss")==true)||(Session.Contents("Ismanager")==true)||(Session.Contents("Isbuch")==true)||(Session.Contents("Isbank")==true)||(Session.Contents("Islogist")==true)) 
	{
		result+='<li><a href="repPriceChart.asp" target="_blank">График цен</a>';
		result+='<li><a href="repPriceChartAdvanced.asp" target="_blank">График цен для аналитиков</a>';
	};
	 if (Session.Contents("Issysadm")==true  || Session.Contents("Istopmanager")==true || Session.Contents("UserId").toLowerCase()=='hrobatenko' ||  Session.Contents("UserId").toLowerCase() =='dudnikova' ||  Session.Contents("UserId").toLowerCase() =='voloshinm' ||  ((Session.Contents("Isboss")==true) && (Session.Contents("Ismanager")==true) && (Session.Contents("IsaccountantCanSee622")==null))) 
	{			
		result+='<li><a href="costincomnewWithoutFarms.asp" target="_blank">Факт и выполнение финплана (БЕЗ ХОЗЯЙСТВ)</a>'; // Прибыль текущая Элеваторы
	}
	if (Session.Contents("Issysadm")==true) {
		//result+='<li><a href="repDebetors.asp" target="_blank">Просроченные дебеторы СТАРЫЙ</a>'; -- отчет удален 2020.04
		//result+='<li><a href="costincom.asp" target="_blank">Доходы и расходы СТАРЫЙ</a>'; -- отчет удален 2020.04
		//result+='<li><a href="costincomCurrent.asp" target="_blank">Прибыль ТЕСТ</a>';
		result+='<li><a href="tech_rep.asp" target="_blank">Технические отчеты</a>'; // добавлено 27.10.2020
		//result+='<li><a href="repExecutedContracts.asp" target="_blank">Прибыль по исполненным контрактам</a>';//added by Viktor 051121	 
		if (Session.Contents("UserId").toLowerCase()=='viktorusr'){
			var flask_url = 'http://192.168.0.208:8080/';
			result+='<li style="display:none"><a id="protected-link"></a>';
			result+='<li><a id="protected-link" href="'+flask_url+'rep-elevators-input" target="_blank">Flask Поступления на элеваторы (нал/безнал) + Хозяйства</a>';
			result+='<li><a id="protected-link" href="'+flask_url+'rep-exp-nds" target="_blank">Flask Просроченный НДС</a>';
			result+='<li><a id="protected-link" href="'+flask_url+'rep-admin-costs" target="_blank">Flask Админ. расходы</a>';
		}
	}
	if (Session.Contents("Istopmanager")==true || ((Session.Contents("Isboss")==true) && (Session.Contents("Ismanager")==true) && (Session.Contents("Isaccountantcansee622")==null)))   //01.02.2022 добавил для трейдеро по просьбе ЛВ
	{	            
		//result+='<li><a href="repExecutedContractsTraderBonusGroups.asp" target="_blank">Бонусы трейдерам без учета перемещений с группами контрактов</a>';//added by Viktor 151121
	}
	//if (Session.Contents("Ispricer")==true) 
	//	result+='<li><a href="pricesdraft.asp" target="_blank">Проект закупочных цен</a>';
	//if (Session.Contents("Isforeigner")==true) {result+='<li><a href="divrep.asp">Отчет</a>';
	//};  
  } else{
		result+='<li><a href="costincomnewWithoutFarms2.asp" target="_blank">KPI (БЕЗ ХОЗЯЙСТВ)</a>'; // Прибыль текущая Элеваторы	
  }
} else { 
	//  result+='<li><a href="newuser.asp">New user</a>'; 
	if (url=='default') {result+='<li><a href="login.asp">Регистрация</a>'};
	};
//if (url=='default') {result+='<li><a href="default.asp?style=alt">Другой стиль</a>'};
return result;
}

function WriteStyle() {
Response.write('<link rel="stylesheet" href="'+Session.Contents("Style")+'" type="text/css" title="default" id="default">');
};

function WriteMenu(url) {
Response.write('<div id="menu"><ul>'+BuildMenu(url)+'</ul></div>');
};

function WriteFooter() {
Response.write('<div id="footer"><a href="default.asp">&copy; Прометей.</a></div>');
};
function WriteHeader() {
//Response.write('<div id="header"><img src="images/fnslogo.gif" /></div>');
Response.write('<div id="header"><p class="logo">'+((""+Session.Contents("Database")=="fa")?'Прометей':"Производственная компания")+'</p> <p class="slogan"></p></div>');
};
function trySession()
{
  if (Session.Contents('UserId')==null)
  {
    Response.Redirect("login.asp");
  };
}

function isMSIE () {
   var ua = (''+Request.ServerVariables('HTTP_USER_AGENT')).toLowerCase(); 
   // browser name
   return ( (ua.indexOf('msie') != -1) && (ua.indexOf('opera') == -1) && (ua.indexOf('webtv') == -1) );
};
function addCommas(nStr)
{
	nStr += '';
	x = nStr.split('.');
	x1 = x[0];
	x2 = x.length > 1 ? '.' + x[1] : '';
	var rgx = /(\d+)(\d{3})/;
	while (rgx.test(x1)) {
		x1 = x1.replace(rgx, '$1' + ',' + '$2');
	}
	return x1 + x2;
}
function addZero(i) 
{
    if (i < 10) {
        i = "0" + i;
    }
    return i;
}

function Cash(money) {
money=Math.round(money*100).toString();
if (money<10) return '0.0'+money;
return (money<100)?'0.'+money:''+money.substring(0,money.length-2)+'.'+money.substring(money.length-2,money.length);
}

function cash(money) {
if (isNaN(money)) money = 0;
return addCommas( (money<0)?'-'+Cash(money*-1):Cash(money) );
}

function cash2(money) {
if (isNaN(money)) money = 0;
return (money<0)?'-'+Cash(money*-1):Cash(money) ;
}

function dateToSQL(ds) {
  dt=new Date(Date.parse(ds));
  month=dt.getMonth()+1;
  smonth=(month<10?'0'+month:''+month);
  day=dt.getDate();
  sday=(day<10?'0'+day:''+day);
  return dt.getFullYear()+smonth+sday;
}

function dateToSQLdayEnd(ds) {
    return dateToSQL(ds)+" 23:59:59.997";
}

function dateToStr(ds) {
  dt=new Date(ds);//new Date(Date.parse(ds));
  month=dt.getMonth()+1;
  smonth=(month<10?'0'+month:''+month);
  day=dt.getDate();
  sday=(day<10?'0'+day:''+day);
  return sday+'.'+smonth+'.'+dt.getFullYear();
}

function dateTimeToStr(ds) {
  dt=new Date(ds);//new Date(Date.parse(ds));
  month=dt.getMonth()+1;
  smonth=(month<10?'0'+month:''+month);
  day=dt.getDate();
  sday=(day<10?'0'+day:''+day);
  return sday+'.'+smonth+'.'+dt.getFullYear()+' '+addZero(dt.getHours())+':'+addZero(dt.getMinutes())+':'+addZero(dt.getSeconds());
}

function parseDate(value) {
	// parse date 24.12.2009
	var tmp = value.split(".");
	var dt=new Date();
	dt.setFullYear(tmp[2]);
	dt.setDate(1);
	dt.setMonth(tmp[1]-1);
	dt.setDate(tmp[0]);
	return dt;
}

function getRS(id){
var rs = Session.Contents('rs_'+id);
if (rs==null) 
{
  var conn=getConnection();
  conn.CommandTimeout = 0;
  rs=conn.Execute(id);
  Session.Contents('rs_'+id)=rs;
};
return rs;
};

function getAcc() {return getRS('select acc,name from ownacc order by acc');};

function getAnal() {return getRS('select * from anal order by anal');};

function getDivis() {return getRS('select * from ownfirm order by id');};
</SCRIPT>
