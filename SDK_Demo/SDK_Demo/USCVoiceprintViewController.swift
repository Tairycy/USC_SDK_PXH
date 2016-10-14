//
//  USCVoiceprintViewController.swift
//  SDK_Demo
//
//  Created by pxh on 2016/10/13.
//  Copyright © 2016年 pxh. All rights reserved.
//

import UIKit

class USCVoiceprintViewController: UIViewController {
    @IBOutlet weak var segmentedControl: UISegmentedControl!

    @IBOutlet weak var textView: UITextView!
    @IBOutlet weak var progress: UIProgressView!
    @IBOutlet weak var downLoad: UIButton!
    @IBOutlet weak var startBtn: UIButton!
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        setUpUI()
    }
    func setUpUI(){
        self.textView.backgroundColor = UIColor.white
        self.textView.layer.borderWidth = 1.0
        self.textView.layer.borderColor = UIColor.black.cgColor
    }
    @IBAction func downLoad(_ sender: AnyObject) {
        print("downLoad")
    }
    @IBAction func startAction(_ sender: UIButton) {
        print("startAction")
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
