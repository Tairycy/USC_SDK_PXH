//
//  USCSetViewController.swift
//  SDK_Demo
//
//  Created by pxh on 2016/10/13.
//  Copyright © 2016年 pxh. All rights reserved.
//

import UIKit

class USCSetViewController: UIViewController,UIPickerViewDelegate,UIPickerViewDataSource {
    var selectedSample : String? = "16000"


    var selectedEngine : String? = "poi"


    var selectedLanguage : String? = "chinese"



    
    @IBOutlet weak var pickerView: UIPickerView!
    let sampleArray = ["16K","8K","自动"]
    let engineArray = ["地名","通用","歌名","医药","影视","多领域"]
    let langungeArray = ["普通话","英语","粤语"]
    

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
//        initData()
        self.setPickerView()
    }
//    func initData(){
//        self.selectedSample = "16000"
//        self.selectedEngine = "poi"
//        self.selectedLanguage = "chinese"
//    }
    func setPickerView(){
        self.pickerView.delegate = self
        self.pickerView.dataSource = self
    }
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    //MARK: - pickerView datasource
    func numberOfComponents(in pickerView: UIPickerView) -> Int{
        return 3
    }
    func pickerView(_ pickerView: UIPickerView, numberOfRowsInComponent component: Int) -> Int{
        switch component {
        case 0:
            return sampleArray.count
        case 1:
            return engineArray.count
        case 2:
            return langungeArray.count
        default:
            break
        }
        return 0;
    }
    //MARK: - pickerView delegate
    func pickerView(_ pickerView: UIPickerView, didSelectRow row: Int, inComponent component: Int){
        switch component {
        case 0:
            let newValue : String = self.sampleArray[row]
            if newValue == "16k"  {
                self.selectedSample = "16000"
            }else if newValue == "8k"{
                self.selectedSample = "8000"
            }else{
                self.selectedSample = "0"
            }
        case 1:
            let newValue : String = self.engineArray[row]
            if newValue == "影视"{
                self.selectedEngine = "movietv"
            }else if newValue == "地名"{
                self.selectedEngine = "poi"
            }else if newValue == "歌名"{
                self.selectedEngine = "song"
            }else if newValue == "多领域"{
                self.selectedEngine = "poi,song"
            }else if newValue == "医药"{
                self.selectedEngine = "medical"
            }else{
                self.selectedEngine = "general"
            }
        case 2:
            let newValue : String = self.langungeArray[row]
            if newValue == "英语"{
                self.selectedLanguage = "english"
            }else if newValue == "粤语"{
                self.selectedLanguage = "cantoness"
            }else{
                self.selectedLanguage = "chinese"
            }
        default:
            break
        }
    }
    func pickerView(_ pickerView: UIPickerView, titleForRow row: Int, forComponent component: Int) -> String?{
        switch component {
        case 0:
            return sampleArray[row]
        case 1:
            return engineArray[row]
        case 2:
            return langungeArray[row]
        default:
            break
        }
        return nil;
    }
    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
