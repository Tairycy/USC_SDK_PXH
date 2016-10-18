//
//  USCAsrViewController.swift
//  SDK_Demo
//
//  Created by pxh on 2016/10/13.
//  Copyright © 2016年 pxh. All rights reserved.
//

import UIKit

class USCAsrViewController: UIViewController,USCSpeechUnderstanderDelegate,USCSpeechSynthesizerDelegate {
    @IBOutlet weak var progress: UIProgressView!
    @IBOutlet weak var textView: UITextView!
    @IBOutlet weak var startBtn: UIButton!
    @IBOutlet weak var uploadBtn: UIButton!

    var asrResult : NSMutableString?
    var jsonResult : NSMutableString?
    
    let setVC : USCSetViewController = USCSetViewController()
    
    /****SpeechUnderstander*****/
    var speechUnderstander : USCSpeechUnderstander?
    var speechSynthesizer : USCSpeechSynthesizer?
    var speechResult : USCSpeechResult?

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        setUpSpeech()
        setUpUI()
        self.setNavi()
    }
    func setUpUI(){
        self.startBtn.setTitle("开始", for: UIControlState.normal)
        self.startBtn.setTitle("停止", for: UIControlState.selected)
        self.textView.text = ""
        self.textView.layer.borderColor = UIColor.black.cgColor
        self.textView.layer.borderWidth = 1.0
        self.textView.backgroundColor = UIColor.white
        self.asrResult = NSMutableString.init()
        self.jsonResult = NSMutableString.init()
    }
    func uploadAudio(_ sender : AnyObject){
        let glassPath = Bundle.main.path(forResource: "16k", ofType: "wav")
        self.speechUnderstander?.recognizeAudioFile(glassPath)
        let city = UserDefaults.standard.object(forKey: "currentcity")
        print(city)
    }
    func reset(){
        self.speechUnderstander?.stop()
        self.asrResult = ""
        self.jsonResult = ""
    }
    //MARK:- SpeechUnderstander
    func setUpSpeech(){
        let recorder = USCRecorder.init()
        let playThread = USCPlayThread.init()
        //creat speechUnderstander
        self.speechUnderstander = USCSpeechUnderstander.init(context: nil, appKey: APPKEY, secret: SECRET)
        self.speechUnderstander?.setOption(USC_ASR_DOMAIN, value: "poi")
        
        self.speechUnderstander?.setOption(USC_ASR_VAD_TIMEOUT_FRONTSIL, value: "3000")
        self.speechUnderstander?.setOption(USC_ASR_VAD_TIMEOUT_BACKSIL, value: "1000")
        self.speechUnderstander?.setOption(USC_NLU_ENABLE, value: "true")
        self.speechUnderstander?.setOption(USC_ASR_LOCATION_ENABLE, value: "true")
        self.speechUnderstander?.setOption(USC_ASR_SERVER_ADDR, value: "vpr.hivoice.cn:80")   //设置私有服务器识别地址
        self.speechUnderstander?.setOption(USC_ASR_OPT_FRONT_VAD_ENABLED, value: "false")     //控制vad是否可用
        self.speechUnderstander?.setOption(USC_NLU_SCENARIO, value: "incar")
        self.speechUnderstander?.delegate = self
        self.speechUnderstander?.setAudioSource(recorder)
        
        //create speechSynthesize
        self.speechSynthesizer = USCSpeechSynthesizer.init(appkey: APPKEY, secret: SECRET)
        self.speechSynthesizer?.delegate = self
        self.speechSynthesizer?.setOption(USC_TTS_KEY_VOICE_VOLUME, value: "100")
        self.speechSynthesizer?.setOption(USC_TTS_KEY_VOICE_NAME, value: "xiaoli")
        self.speechSynthesizer?.setAudioSource(playThread)
    }
    func setNavi(){
        self.title = "云知声语音理解"
    }
    @IBAction func startAction(_ sender: UIButton) {
        //E-SETVC
        self.speechUnderstander?.setOption(USC_ASR_LANGUAGE, value: setVC.selectedLanguage)
        self.speechUnderstander?.setOption(USC_ASR_SAMPLING_RATE, value: setVC.selectedSample)
        self.speechUnderstander?.setOption(USC_ASR_DOMAIN, value: setVC.selectedEngine)
        print("setVC selectedLanguage:%@ selectedSample:%@ selectedEngine:%@", setVC.selectedLanguage,setVC.selectedSample,setVC.selectedEngine)
        print("startAction")
        if sender.isSelected {
            self.speechUnderstander?.stop()
        }else{

            reset()
            self.speechUnderstander?.start()
        }
    }
    @IBAction func upLoadAction(_ sender: AnyObject) {
        print("upLoadAction")
        reset()
        uploadAudio(sender)
    }
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    //MARK:-  USCSpeechUnderstanderDelegate
    func onError(_ type: Int32, error: Error!) {
        print("onError type:%d ",type)
        if (error != nil) {
            print("error = %@",error)
            return;
        }
        if (USC_TTS_ERROR == type) {
            return;
        }
        self.startBtn.isSelected = false
        if ((asrResult?.length)! > 0) {
            print("播放的文字：%@",self.asrResult)
            self.speechSynthesizer?.playText(self.asrResult! as String!)
        }
        self.startBtn.isSelected = false
        self.progress.progress = 0.0
    }
    func onEvent(_ type: Int32, timeMs: Int32) {
        print("onEvent type:%d timeMs:%d",type,timeMs)
        if (type == USC_ASR_EVENT_VOLUMECHANGE) {
            let volumeRate : NSInteger = self.speechUnderstander?.getOption(USC_ASR_EVENT_VOLUMECHANGE) as! NSInteger
            let ratiProgress : Float = Float(volumeRate) / 100.0
            self.progress.setProgress(ratiProgress, animated: true)
            print("ratiProgress : %f",ratiProgress)
        }
        if (USC_ASR_EVENT_RECORDING_START == type) {
            self.startBtn.isSelected = true
        }
    }
    func onResult(_ type: Int32, jsonString: String!) {
        NSLog("type = %d json = %@",type, jsonString)
        /*注意这个方法会调用多次，注意根据type判断，得到想要的结果*/
        if(USC_ASR_RESULT_NET == type){
            self.jsonResult?.append(jsonString)
        }
        self.textView.text = self.jsonResult! as String
    }
    //MARK:-  USCSpeechSynthesizerDelegate
    func onEvent(_ type: Int32) {
        
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
