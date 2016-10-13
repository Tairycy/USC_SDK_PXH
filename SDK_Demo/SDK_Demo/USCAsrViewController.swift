//
//  USCAsrViewController.swift
//  SDK_Demo
//
//  Created by pxh on 2016/10/13.
//  Copyright © 2016年 pxh. All rights reserved.
//

import UIKit

class USCAsrViewController: UIViewController {
    @IBOutlet weak var progress: UIProgressView!
    @IBOutlet weak var textView: UITextView!
    @IBOutlet weak var startBtn: UIButton!
    @IBOutlet weak var uploadBtn: UIButton!

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        setUpUI()
        self.setNavi()
    }
    func setUpUI(){
        self.startBtn.setTitle("开始", for: UIControlState.normal)
        self.startBtn.setTitle("停止", for: UIControlState.selected)
        self.textView.layer.borderColor = UIColor.black.cgColor
        self.textView.layer.borderWidth = 1.0
        self.textView.backgroundColor = UIColor.white
    }
    
    func setNavi(){
        self.title = "云知声语音理解"
    }
    @IBAction func startAction(_ sender: UIButton) {
        sender.isSelected = !sender.isSelected
        print("startAction")
    }
    @IBAction func upLoadAction(_ sender: AnyObject) {
        print("upLoadAction")
    }
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
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
